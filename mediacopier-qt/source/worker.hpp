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

#include "config.hpp"
#include "status.hpp"

#ifdef ENABLE_KDE
#include <KUiServerV2JobTracker>
#endif

#include <QThread>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker() = delete;
    Worker(Config config);

    void start();
    void suspend();
    void resume();
    void kill();

Q_SIGNALS:
    void status(Status info);
    void execDone();
    void finished();

public Q_SLOTS:
    void exec();
    void quit();

private:
    const Config m_config;
    QThread m_thread;

#ifdef ENABLE_KDE
    KUiServerV2JobTracker m_tracker;
#endif
};
