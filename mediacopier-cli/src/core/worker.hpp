/* Copyright (C) 2021-2022 Patrick Ziegler
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

#include "core/config.hpp"
#include "core/status.hpp"

#include <KUiServerV2JobTracker>

#include <QThread>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker() = delete;
    Worker(Config config);

    void start();
    void kill();
    void suspend();
    void resume();

Q_SIGNALS:
    void status(Status info);
    void execDone();
    void finished();

public Q_SLOTS:
    void exec();
    void quit();

private:
    QThread m_thread;
    Config m_config;
    KUiServerV2JobTracker m_tracker;
};
