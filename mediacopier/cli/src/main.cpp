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
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>

#include "config.hpp"

namespace fs = std::filesystem;

using namespace MediaCopier;

template <typename T>
void execute(const FileRegister& fileRegister)
{
    for (const auto& element : fileRegister) {
        T operation{element.first};
        element.second->accept(operation);
    }
}

void run(const ConfigManager& config)
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("run"));

    if (!fs::is_directory(config.inputDir())) {
        throw std::runtime_error("Input folder does not exist");
    }

    FileRegister fileRegister{config.outputDir(), config.baseFormat()};

    for (const auto& path : fs::recursive_directory_iterator(config.inputDir())) {
        if (path.is_regular_file()) {
            try {
                fileRegister.add(path);
            } catch (const FileInfoError& err) {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(err.what()));
            }
        }
    }

    if (fileRegister.size() < 1) {
        throw std::runtime_error("No files were found in " + config.inputDir().string());
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Found " + std::to_string(fileRegister.size()) + " files in " + config.inputDir().string()));

    switch (config.command())
    {
    case ConfigManager::Command::COPY:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Copying files"));
        execute<FileOperationCopyJpeg>(fileRegister);

    case ConfigManager::Command::MOVE:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Moving files"));
        execute<FileOperationMoveJpeg>(fileRegister);

    default:
        throw std::runtime_error("Unknown operation type");
    }
}

int main(int argc, char *argv[])
{
    log4cplus::BasicConfigurator log;
    log.configure();

    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    ConfigManager config;
    config.parseArgs(argc, argv);

    try {
        run(config);
    } catch (const std::exception& err) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(err.what()));
        return 1;
    }

    return 0;
}
