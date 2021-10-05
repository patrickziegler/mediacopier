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

#include "ConfigManager.hpp"

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/Error.hpp>
#include <mediacopier/FileOperationMove.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>

#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

static volatile std::atomic<bool> operationCancelled;

static auto abortable_wrapper(std::function<void()> abortable_function) -> void
{
    operationCancelled.store(false);
    std::signal(SIGINT, [](int signal) -> void {
        (void) signal;
        operationCancelled.store(true);
    });
    abortable_function();
    std::signal(SIGINT, SIG_DFL);
}

namespace MediaCopier::Cli {

template <typename T>
static auto execute_operation(const FileRegister& fileRegister) -> void
{
    for (const auto& [destination, file] : fileRegister) {
        try {
            T operation{destination};
            file->accept(operation);

        }  catch (const FileOperationError& err) {
            spdlog::warn(std::string{err.what()} + ": " + file->path().string());
        }

        if (operationCancelled.load()) {
            spdlog::info("Aborting execution");
            break;
        }
    }
}

static auto run(const fs::path& inputDir, const fs::path& outputDir, const std::string& pattern, ConfigManager::Command command) -> int
{
    if (!fs::is_directory(inputDir)) {
        throw MediaCopierError("Input folder does not exist");
    }

    FileRegister fileRegister{outputDir, pattern};

    for (const auto& file : fs::recursive_directory_iterator(inputDir)) {
        if (file.is_regular_file()) {
            try {
                fileRegister.add(file);
            } catch (const FileInfoError& err) {
                spdlog::warn(std::string{err.what()} + ": " + file.path().string());
            }
        }
    }

    if (fileRegister.size() < 1) {
        throw MediaCopierError("No files were found in " + inputDir.string());
    }

    spdlog::info("Found " + std::to_string(fileRegister.size()) + " files");

    switch (command)
    {
    case ConfigManager::Command::COPY:
        spdlog::info("Executing COPY operation");
        abortable_wrapper([fileRegister]() -> void {
            execute_operation<FileOperationCopy>(fileRegister);
        });
        break;

    case ConfigManager::Command::MOVE:
        spdlog::info("Executing MOVE operation");
        abortable_wrapper([fileRegister]() -> void {
            execute_operation<FileOperationMove>(fileRegister);
        });
        break;

    case ConfigManager::Command::COPY_JPEG:
        spdlog::info("Executing COPY operation (with JPEG awareness)");
        abortable_wrapper([fileRegister]() -> void {
            execute_operation<FileOperationCopyJpeg>(fileRegister);
        });
        break;

    case ConfigManager::Command::MOVE_JPEG:
        spdlog::info("Executing MOVE operation (with JPEG awareness)");
        abortable_wrapper([fileRegister]() -> void {
            execute_operation<FileOperationMoveJpeg>(fileRegister);
        });
        break;

    default:
        throw MediaCopierError("Unknown operation type");
    }

    spdlog::info("Execution finished");
    return 0;
}

} // namespace MediaCopier::Cli

int main(int argc, char *argv[])
{
    using namespace MediaCopier::Cli;

    try {
        ConfigManager config{argc, argv};
        return run(config.inputDir, config.outputDir, config.pattern, config.command);
    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
