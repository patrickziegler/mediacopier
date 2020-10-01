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

#include <mediacopier/cli/ConfigStore.hpp>
#include <mediacopier/cli/execute.hpp>

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FilePathFactory.hpp>

#include <log4cplus/loggingmacros.h>

#include <filesystem>

namespace fs = std::filesystem;

namespace MediaCopier::Cli {

int execute(const ConfigStore& config)
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("execute"));

    MediaCopier::FilePathFactory filePathFactory{config.outputDir(), config.baseFormat()};
    std::unique_ptr<MediaCopier::AbstractFileOperation> op;

    switch (config.command()) {
    case ConfigStore::Command::COPY:
        op = std::make_unique<MediaCopier::FileOperationCopyJpeg>(filePathFactory);
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Using 'copy' operation"));
        break;

    case ConfigStore::Command::MOVE:
        op = std::make_unique<MediaCopier::FileOperationMoveJpeg>(filePathFactory);
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Using 'move' operation"));
        break;

    default:
        return 0;
    }

    MediaCopier::FileInfoFactory fileInfoFactory;

    for (const auto& path : fs::recursive_directory_iterator(config.inputDir())) {
        try {
            if (path.is_regular_file()) {
                auto file = fileInfoFactory.createFileFrom(path);
                file->accept(*op);
            }
        }  catch (const MediaCopier::FileInfoError& err) {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(err.what() << " (" << path << ")"));
        }
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Operation completed"));
    return 0;
}

}
