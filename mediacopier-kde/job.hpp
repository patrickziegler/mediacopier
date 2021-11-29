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

#pragma once

#include "worker.hpp"

#include <KJob>
#include <QThread>

class MediaCopierJob : public KJob
{
    Q_OBJECT

public:
    MediaCopierJob(const Worker::Command& command,
                   const std::filesystem::path& srcDir,
                   const std::filesystem::path& dstDir,
                   const std::string& pattern);

    void start() override;

public Q_SLOTS:
    void update(Status info);
    void quit();

protected:
    bool doKill() override;
    bool doSuspend() override;
    bool doResume() override;

private:
    QThread m_thread;
    Worker m_worker;
};
