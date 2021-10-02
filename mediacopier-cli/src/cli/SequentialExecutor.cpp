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

#include "SequentialExecutor.hpp"

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/Error.hpp>
#include <mediacopier/FileOperationMove.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>

#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>
#include <functional>

namespace fs = std::filesystem;

static volatile std::atomic<bool> operationCancelled;

static void abortable_wrapper(std::function<void()> abortable_function)
{
    operationCancelled.store(false);
    std::signal(SIGINT, [](int signal) -> void {
        (void) signal;
        operationCancelled.store(true);
    });
    abortable_function();
    std::signal(SIGINT, SIG_DFL);
}

template <typename T>
static void execute(const MediaCopier::FileRegister& fileRegister)
{
    for (const auto& [destination, file] : fileRegister) {
        try {
            T operation{destination};
            file->accept(operation);

        }  catch (const MediaCopier::FileOperationError& err) {
            spdlog::warn(std::string{err.what()} + ": " + file->path().string());
        }

        if (operationCancelled.load()) {
            spdlog::info("Aborting execution");
            break;
        }
    }
}

namespace MediaCopier::Cli {

void SequentialExecutor::run() const
{
    if (!fs::is_directory(m_config.inputDir)) {
        throw MediaCopierError("Input folder does not exist");
    }

    FileRegister fileRegister{m_config.outputDir, m_config.pattern};

    for (const auto& file : fs::recursive_directory_iterator(m_config.inputDir)) {
        if (file.is_regular_file()) {
            try {
                fileRegister.add(file);
            } catch (const FileInfoError& err) {
                spdlog::warn(std::string{err.what()} + ": " + file.path().string());
            }
        }
    }

    if (fileRegister.size() < 1) {
        throw MediaCopierError("No files were found in " + m_config.inputDir.string());
    }

    spdlog::info("Found " + std::to_string(fileRegister.size()) + " files");

    switch (m_config.command)
    {
    case ConfigManager::Command::COPY:
        spdlog::info("Executing COPY operation");
        abortable_wrapper([fileRegister]() -> void {
            execute<FileOperationCopy>(fileRegister);
        });
        break;

    case ConfigManager::Command::MOVE:
        spdlog::info("Executing MOVE operation");
        abortable_wrapper([fileRegister]() -> void {
            execute<FileOperationMove>(fileRegister);
        });
        break;

    case ConfigManager::Command::COPY_JPEG:
        spdlog::info("Executing COPY operation (with JPEG awareness)");
        abortable_wrapper([fileRegister]() -> void {
            execute<FileOperationCopyJpeg>(fileRegister);
        });
        break;

    case ConfigManager::Command::MOVE_JPEG:
        spdlog::info("Executing MOVE operation (with JPEG awareness)");
        abortable_wrapper([fileRegister]() -> void {
            execute<FileOperationMoveJpeg>(fileRegister);
        });
        break;

    default:
        throw MediaCopierError("Unknown operation type");
    }

    spdlog::info("Execution finished");
}

} // namespace MediaCopier::Cli
