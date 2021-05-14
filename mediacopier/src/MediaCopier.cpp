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
#include <mediacopier/Error.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>
#include <mediacopier/MediaCopier.hpp>

#include <log4cplus/log4cplus.h>

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
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("execute"));

    for (const auto& [destination, file] : fileRegister) {
        try {
            T operation{destination};
            file->accept(operation);

        }  catch (const MediaCopier::FileOperationError& err) {
            LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(std::string{err.what()} + ": " + file->path().string()));
        }

        if (operationCancelled.load()) {
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Aborting execution"));
            break;
        }
    }
}

namespace MediaCopier {

void MediaCopier::run()
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("run"));

    if (!fs::is_directory(m_inputDir)) {
        throw MediaCopierError("Input folder does not exist");
    }

    FileRegister fileRegister{m_outputDir, m_pattern};

    for (const auto& file : fs::recursive_directory_iterator(m_inputDir)) {
        if (file.is_regular_file()) {
            try {
                fileRegister.add(file);
            } catch (const FileInfoError& err) {
                LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(std::string{err.what()} + ": " + file.path().string()));
            }
        }
    }

    if (fileRegister.size() < 1) {
        throw MediaCopierError("No files were found in " + m_inputDir.string());
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Found " + std::to_string(fileRegister.size())) + " files");

    switch (m_command)
    {
    case Command::COPY:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Executing COPY operation"));
        abortable_wrapper([fileRegister]() -> void {
            execute<FileOperationCopyJpeg>(fileRegister);
        });
        break;

    case Command::MOVE:
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Executing MOVE operation"));
        abortable_wrapper([fileRegister]() -> void {
            execute<FileOperationMoveJpeg>(fileRegister);
        });
        break;

    default:
        throw MediaCopierError("Unknown operation type");
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Execution finished"));
}

} // namespace MediaCopier
