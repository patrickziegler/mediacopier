/* Copyright (C) 2020 Patrick Ziegler
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

#include "config.hpp"
#include "feedback.hpp"

#include <atomic>
#include <csignal>

namespace cli = MediaCopier::Cli;
namespace fs = std::filesystem;

static constexpr const size_t PROGRESS_UPDATE_INTERVAL_MS = 500;

static volatile std::atomic<bool> operationCancelled;

using namespace MediaCopier;
using namespace MediaCopier::Cli;

void onSigInt(int s)
{
    (void) s;
    operationCancelled.store(true);
}

int run(const cli::ConfigManager& config, cli::FeedbackProxy& feedback)
{
    if (!fs::is_directory(config.inputDir())) {
        feedback.log(cli::LogLevel::ERROR, "Input folder does not exist");
        return 1;
    }

    FilePathFactory filePathFactory{config.outputDir(), config.baseFormat()};

    std::unique_ptr<AbstractFileOperation> op;

    size_t count = std::distance(
                fs::recursive_directory_iterator{config.inputDir()},
                fs::recursive_directory_iterator{});

    if (count < 1) {
        feedback.log(LogLevel::WARNING, "No files were found in " + config.inputDir().string());
        return 0;
    }

    feedback.log(LogLevel::INFO, "Found " + std::to_string(count) + " files");

    switch (config.command())
    {
    case Command::COPY:
        op = std::make_unique<FileOperationCopyJpeg>(filePathFactory);
        feedback.log(LogLevel::INFO, "Copying files");
        break;

    case Command::MOVE:
        op = std::make_unique<FileOperationMoveJpeg>(filePathFactory);
        feedback.log(LogLevel::INFO, "Moving files");
        break;

    default:
        feedback.log(LogLevel::ERROR, "Unknown operation type");
        return 1;
    }

    auto execute = [&config, &feedback, &op, &count]() -> void {
        auto clock = std::chrono::steady_clock();
        auto ref = clock.now();

        size_t diff = 0;
        size_t pos = 0;

        FileInfoFactory fileInfoFactory;

        feedback.progress(0);

        for (const auto& path : fs::recursive_directory_iterator(config.inputDir())) {
            try {
                if (path.is_regular_file()) {
                    auto file = fileInfoFactory.createFromPath(path);
                    file->accept(*op);
                }
            }  catch (const FileInfoError& err) {
                feedback.log(LogLevel::WARNING, std::string{err.what()} + " in '" + path.path().string() + "'");
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

        feedback.progress(100 * pos / count);
    };

    operationCancelled.store(false);

    std::signal(SIGINT, onSigInt);

    try {
        execute();
    } catch (const std::exception& err) {
        feedback.log(LogLevel::ERROR, err.what());
    }

    std::signal(SIGINT, SIG_DFL);

    return 0;
}


#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>


namespace cli = MediaCopier::Cli;
namespace fs  = std::filesystem;

int main(int argc, char *argv[])
{
    log4cplus::BasicConfigurator log;
    log.configure();

    cli::ConfigManager config;
    config.parseArgs(argc, argv);

    cli::FeedbackProxy feedback;

    try {
        return run(config, feedback);
    } catch (const std::exception& err) {
        feedback.log(cli::LogLevel::ERROR, err.what());
    }

    return 1;
}
