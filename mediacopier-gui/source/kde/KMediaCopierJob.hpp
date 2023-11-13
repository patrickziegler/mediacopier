/* Copyright (C) 2021 Patrick Ziegler <zipat@proton.me>
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

#include "status.hpp"

#include <KJob>

class Worker;

class KMediaCopierJob : public KJob {
    Q_OBJECT

public:
    KMediaCopierJob(
            Worker* worker,
            const std::filesystem::path& dstDir);
    void start() override;

public Q_SLOTS:
    void update(Status info);
    void quit();

protected:
    bool doKill() override;
    bool doSuspend() override;
    bool doResume() override;

private:
    Worker* m_worker;
};
