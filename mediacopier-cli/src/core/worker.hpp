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

#include "config.hpp"

#include <QThread>

class Status {
public:
    Status() {};
    Status(QString command,
           std::filesystem::path inputPath,
           std::filesystem::path outputPath,
           size_t fileCount,
           size_t progress) :
        m_command{std::move(command)},
        m_inputPath{std::move(inputPath)},
        m_outputPath{std::move(outputPath)},
        m_fileCount{std::move(fileCount)},
        m_progress{std::move(progress)}
    {
        // nothing to do here
    }

    const QString& command() const {
        return m_command;
    }

    const std::filesystem::path& inputPath() const {
        return m_inputPath;
    }

    const std::filesystem::path& outputPath() const {
        return m_outputPath;
    }

    const size_t& fileCount() const {
        return m_fileCount;
    }

    const size_t& progress() const {
        return m_progress;
    }

private:
    const QString m_command = "";
    const std::filesystem::path m_inputPath = "";
    const std::filesystem::path m_outputPath = "";
    const size_t m_fileCount = 0;
    const size_t m_progress = 0;
};

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
};