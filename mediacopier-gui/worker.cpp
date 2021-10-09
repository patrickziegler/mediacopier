/* Copyright (C) 2020-2021 Patrick Ziegler
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

#include "worker.hpp"

#include <mediacopier/abstract_file_info.hpp>
#include <mediacopier/error.hpp>
#include <mediacopier/file_operation_move.hpp>
#include <mediacopier/file_operation_move_jpeg.hpp>
#include <mediacopier/file_operation_simulate.hpp>
#include <mediacopier/file_register.hpp>

#include <spdlog/spdlog.h>

#include <filesystem>

namespace fs = std::filesystem;

using FileInfoPtr = std::shared_ptr<mediacopier::AbstractFileInfo>;

static volatile std::atomic<bool> operationCancelled;

template <typename T>
static void execute(const FileInfoPtr& file, const std::string& destination)
{
    T operation{destination};
    file->accept(operation);
}

Worker::Worker()
{
    // TODO: init spdlog to use emit emit appendLog here
}

void Worker::onOperationStarted()
{
    using namespace mediacopier;

    try {
        fs::path inputDir{m_inputDir.toStdString()};
        fs::path outputDir{m_outputDir.toStdString()};

        // copy value to prevent unwanted modification during execution
        auto command = m_command;

        if (!fs::is_directory(inputDir)) {
            throw MediaCopierError("Input folder does not exist");
        }

        FileRegister fileRegister{outputDir, m_pattern.toStdString()};

        for (const auto& file : fs::recursive_directory_iterator(inputDir)) {
            if (file.is_regular_file()) {
                try {
                    fileRegister.add(file);
                } catch (const FileInfoError& err) {
                    spdlog::warn(std::string{err.what()} + ": " + file.path().string());
                }
            }
        }

        emit resetProgress(fileRegister.size());
        operationCancelled.store(false);

        for (const auto& [destination, file] : fileRegister) {

            if (operationCancelled.load()) {
                spdlog::info("Aborting execution");
                break;
            }

            try {
                switch (command) {
                case Command::COPY:
                    execute<FileOperationCopy>(file, destination);
                    break;
                case Command::COPY_JPEG:
                    execute<FileOperationCopyJpeg>(file, destination);
                    break;
                case Command::MOVE:
                    execute<FileOperationMove>(file, destination);
                    break;
                case Command::MOVE_JPEG:
                    execute<FileOperationMoveJpeg>(file, destination);
                    break;
                case Command::SIMULATE:
                    execute<FileOperationSimulate>(file, destination);
                    break;
                }
            }  catch (const FileOperationError& err) {
                spdlog::warn(std::string{err.what()} + ": " + file->path().string());
            }

            emit bumpProgress();
        }

    } catch (const std::exception& err) {
        emit appendLog(QString{"ERROR: %1"}.arg(err.what()));
    }

    emit operationFinished();
}

void Worker::cancelOperation()
{
    operationCancelled.store(true);
}
