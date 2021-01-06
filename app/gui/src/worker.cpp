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

#include <mediacopier/cli/run.hpp>
#include <mediacopier/gui/worker.hpp>

#include <csignal>
#include <iostream>

namespace cli = MediaCopier::Cli;

void Worker::log(cli::LogLevel level, std::string message)
{
    switch (level)
    {
    case cli::LogLevel::INFO:
        emit logInfoMessage(QString::fromStdString(message));
        break;

    case cli::LogLevel::WARNING:
        emit logWarningMessage(QString::fromStdString(message));
        break;

    default:
        emit logErrorMessage(QString::fromStdString(message));
    }
}

void Worker::progress(size_t value)
{
    emit progressValue(static_cast<int>(value));
}

void Worker::start()
{
    try {
        cli::run(m_config, *this);
    } catch (const std::exception& err) {
        emit logErrorMessage(err.what());
    }
    emit finishedSignal();
}

void Worker::cancel()
{
    std::raise(SIGINT);
}
