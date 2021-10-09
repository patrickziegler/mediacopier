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

#include <chrono>
#include <thread>

static volatile std::atomic<bool> operationCancelled;

void Worker::onOperationStarted()
{
    try {
        operationCancelled.store(false);

        size_t n = 10;
        emit resetProgress(n);

        emit appendLog("input dir: " + m_inputDir);
        emit appendLog("output dir: " + m_outputDir);
        emit appendLog("pattern: " + m_pattern);

        switch (m_command) {
        case Command::COPY_JPEG:
            emit appendLog("copy");
            break;
        case Command::MOVE_JPEG:
            emit appendLog("move");
            break;
        case Command::SIMULATE:
            emit appendLog("sim");
            break;
        default:
            emit appendLog("Unkown operation");
            goto done;
        }

        for (size_t i = 0; i < n; ++i) {
            emit appendLog("iteration");
            if (operationCancelled.load()) {
                emit appendLog("Operation aborted");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            emit setProgress(i + 1);
        }

    } catch (const std::exception& err) {
        emit appendLog(QString{"[ERROR] %1"}.arg(err.what()));
    }

done:
    emit operationFinished();
}

void Worker::cancelOperation()
{
    operationCancelled.store(true);
}
