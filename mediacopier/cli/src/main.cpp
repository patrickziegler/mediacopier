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

#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>

#include "abort.hpp"
#include "config.hpp"

namespace fs = std::filesystem;

using namespace MediaCopier;

static constexpr const size_t PROGRESS_UPDATE_INTERVAL_MS = 500;

std::unique_ptr<AbstractFileOperation> prepare_operation(const ConfigManager& config)
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("prepare_operation"));

    if (!fs::is_directory(config.inputDir())) {
        throw std::runtime_error("Input folder does not exist");
    }

    FileRegister fileRegister{config.outputDir(), config.baseFormat()};

    switch (config.command())
    {
    case ConfigManager::Command::COPY:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Copying files"));
        return std::make_unique<FileOperationCopyJpeg>(fileRegister);

    case ConfigManager::Command::MOVE:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Moving files"));
        return std::make_unique<FileOperationMoveJpeg>(fileRegister);

    default:
        throw std::runtime_error("Unknown operation type");
    }
}

void execute_operation(fs::path inputDir, std::unique_ptr<AbstractFileOperation> op)
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("execute_operation"));

    size_t fileCount = std::distance(
                fs::recursive_directory_iterator{inputDir},
                fs::recursive_directory_iterator{});

    if (fileCount < 1) {
        throw std::runtime_error("No files were found in " + inputDir.string());
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Found " + std::to_string(fileCount) + " files in " + inputDir.string()));

    auto clock = std::chrono::steady_clock();
    auto ref = clock.now();

    size_t diff = 0;
    size_t pos = 0;

    FileInfoFactory fileInfoFactory;

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Progress 0 %"));

    for (const auto& path : fs::recursive_directory_iterator(inputDir)) {

        if (abortable::aborted()) {
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Operation cancelled"));
            break;
        }

        try {
            if (path.is_regular_file()) {
                auto file = fileInfoFactory.createFromPath(path);
                file->accept(*op);
            }
        } catch (const FileInfoError& err) {
            LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(std::string{err.what()} + " in '" + path.path().string() + "'"));
        }

        ++pos;

        diff = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - ref).count();

        if (diff > PROGRESS_UPDATE_INTERVAL_MS) {
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Progress " << 100 * pos / fileCount << " %"));
            ref = clock.now();
        }
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Progress " << 100 * pos / fileCount << " %"));
}

int main(int argc, char *argv[])
{
    log4cplus::BasicConfigurator log;
    log.configure();

    ConfigManager config;
    config.parseArgs(argc, argv);

    auto run = [config]() -> int {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("run"));

        try {
            auto op = prepare_operation(config);
            execute_operation(config.inputDir(), std::move(op));
        } catch (const std::exception& err) {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(err.what()));
            return 1;
        }

        return 0;
    };

    return abortable::wrapper(run);
}
