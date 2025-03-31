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

#include <QThread>

#include <filesystem>

struct StatusDescription {
    std::filesystem::path inputPath;
    std::filesystem::path outputPath;
};

struct StatusProgress {
    size_t count;
    size_t progress;
};

class Worker : public QObject {
    Q_OBJECT

public:
    Worker() = delete;
    Worker(std::shared_ptr<Config> config);

    void start();
    void suspend();
    void resume();
    void kill();

Q_SIGNALS:
    void updateDescription(StatusDescription info);
    void updateProgress(StatusProgress info);
    void execDone();
    void finished();

public Q_SLOTS:
    void exec();
    void quit();

protected:
    std::shared_ptr<Config> m_config;
    QThread m_thread;
};

class WorkerFactory {
public:
    WorkerFactory() = delete;
    explicit WorkerFactory(std::shared_ptr<Config> config) : m_config{std::move(config)} {}
    virtual ~WorkerFactory() {}
    virtual std::unique_ptr<Worker> make_worker(const QString& /* description */) {
        return std::make_unique<Worker>(m_config);
    }
protected:
    std::shared_ptr<Config> m_config;
};
