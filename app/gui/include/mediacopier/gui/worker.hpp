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

#pragma once

#include <QObject>

#include <mediacopier/cli/feedback.hpp>

class Worker : public QObject, private MediaCopier::Cli::FeedbackProxy {
    Q_OBJECT

public:
    Worker(const MediaCopier::Cli::ConfigManager& config) : m_config{config} {};
    Worker(const MediaCopier::Cli::ConfigManager&&) = delete; // don't accept temporary references
    void log(MediaCopier::Cli::LogLevel level, std::string message) override;
    void progress(size_t value) override;

public slots:
    void start();
    void cancel();

signals:
    void logInfoMessage(QString message);
    void logWarningMessage(QString message);
    void logErrorMessage(QString message);
    void progressValue(int value);
    void finishedSignal();

private:
    const MediaCopier::Cli::ConfigManager& m_config;
};
