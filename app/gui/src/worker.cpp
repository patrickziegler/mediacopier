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

#include <mediacopier/gui/worker.hpp>

// TODO: remove
#include <thread>

namespace cli = MediaCopier::Cli;

inline void waitForSomeTime(int secs=1)
{
    std::this_thread::sleep_for(std::chrono::seconds(secs));
}

void Worker::useConfig(cli::ConfigStore config)
{
    m_config = std::move(config);
}

void Worker::run() {
    m_abort = false;
    while (!m_abort) {
        if (m_config.command() == cli::ConfigStore::Command::COPY) {
            emit log("[worker] running copy operation");
        } else {
            emit log("[worker] running other operation");
        }
        waitForSomeTime();
    }
    emit log("[worker] leaving");
    emit finished();
}

void Worker::abort() {
    emit log("[worker] abort");
    m_abort = true;
}
