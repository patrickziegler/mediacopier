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

#include <QObject>

#include <filesystem>

class Status {
public:
    Status() {};
    Status(std::filesystem::path inputPath,
           std::filesystem::path outputPath,
           size_t fileCount,
           size_t progress) :
        m_inputPath{std::move(inputPath)},
        m_outputPath{std::move(outputPath)},
        m_fileCount{std::move(fileCount)},
        m_progress{std::move(progress)}
    {
        // nothing to do here
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
    const std::filesystem::path m_inputPath = "";
    const std::filesystem::path m_outputPath = "";
    const size_t m_fileCount = 0;
    const size_t m_progress = 0;
};

class Worker : public QObject
{
    Q_OBJECT

public:
    enum class Command {
        COPY,
        MOVE,
        COPY_JPEG,
        MOVE_JPEG,
        SHOW
    };

    Worker() = delete;
    Worker(const Command& command,
           std::filesystem::path inputDir,
           std::filesystem::path outputDir,
           std::string pattern = "%Y/%W/IMG_%Y%m%d_%H%M%S");

    size_t fileCount() const { return m_fileCount; };

    bool kill();
    bool suspend();
    bool resume();

private:
    template<typename T> void execute();

public Q_SLOTS:
    void initialize();
    void run();

Q_SIGNALS:
    void status(Status info);
    void initialized();
    void finished();

private:
    const Command m_command;
    const std::filesystem::path m_inputDir;
    const std::filesystem::path m_outputDir;
    const std::string m_pattern;
    size_t m_fileCount = 0;
};
