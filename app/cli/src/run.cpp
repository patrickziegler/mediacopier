/* Copyright (C) 2021 Patrick Ziegler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FilePathFactory.hpp>
#include <mediacopier/cli/run.hpp>

#include <atomic>
#include <signal.h>

namespace cli = MediaCopier::Cli;
namespace fs  = std::filesystem;

static constexpr const size_t PROGRESS_UPDATE_INTERVAL_MS = 500;

static std::atomic<bool> operationCancelled;

void onSignalInterrupt(int s)
{
    (void) s;
    operationCancelled.store(true);
}

void cli::run(const ConfigManager& config, FeedbackGateway& feedback)
{
    auto execute_operation = [&]() -> void {
        std::stringstream ss;

        FilePathFactory filePathFactory{config.outputDir(), config.baseFormat()};
        std::unique_ptr<AbstractFileOperation> op{};

        size_t count = std::distance(
                    fs::recursive_directory_iterator(config.inputDir()),
                    fs::recursive_directory_iterator{});

        if (count < 1) {
            ss << "No files were found in " << config.inputDir();
            feedback.log(LogLevel::WARNING, ss.str());
            return;
        }

        switch (config.command())
        {
        case Command::COPY:
            op = std::make_unique<FileOperationCopyJpeg>(filePathFactory);
            ss << "Copying " << count << " files";
            feedback.log(LogLevel::INFO, ss.str());
            break;

        case Command::MOVE:
            op = std::make_unique<FileOperationMoveJpeg>(filePathFactory);
            ss << "Moving " << count << " files";
            feedback.log(LogLevel::INFO, ss.str());
            break;

        default:
            feedback.log(LogLevel::ERROR, "Unknown operation type");
            return;
        }

        feedback.progress(0);

        auto clock = std::chrono::steady_clock();
        auto ref = clock.now();

        size_t diff = 0;
        size_t pos = 0;

        FileInfoFactory fileInfoFactory{};

        for (const auto& path : fs::recursive_directory_iterator(config.inputDir())) {
            try {
                if (path.is_regular_file()) {
                    auto file = fileInfoFactory.createFromPath(path);
                    file->accept(*op);
                }
            }  catch (const FileInfoError& err) {
                ss.str("");
                ss.clear();
                ss << err.what() << " (" << path.path() << ") ";
                feedback.log(LogLevel::WARNING, ss.str());
            }

            ++pos;

            diff = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - ref).count();

            if (diff > PROGRESS_UPDATE_INTERVAL_MS) {
                feedback.progress(100 * pos / count);
                ref = clock.now();
            }

            if (operationCancelled.load()) {
                feedback.log(LogLevel::INFO, "Operation cancelled");
                break;
            }
        }
    };

    operationCancelled.store(false);

    signal(SIGINT, onSignalInterrupt);

    try {
        execute_operation();
    } catch (const std::exception& err) {
        feedback.log(LogLevel::ERROR, err.what());
    }

    signal(SIGINT, SIG_DFL);
}
