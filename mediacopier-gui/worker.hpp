/* Copyright (C) 2020-2021 Patrick Ziegler
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

class Worker : public QObject
{
    Q_OBJECT

public:
    enum class Command {
        COPY,
        MOVE,
        COPY_JPEG,
        MOVE_JPEG,
        SIMULATE
    };

    Worker() = default;
    void cancelOperation();

public:
    Command m_command = Command::COPY;
    QString m_inputDir = "";
    QString m_outputDir = "";
    QString m_pattern = "%Y/%u/IMG_%Y%m%d_%H%M%S";

public slots:
    void onOperationStarted();

signals:
    void appendLog(QString message);
    void setProgress(int value);
    void resetProgress(int value);
    void operationFinished();
};
