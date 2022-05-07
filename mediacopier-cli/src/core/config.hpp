/* Copyright (C) 2022 Patrick Ziegler
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

#include <QApplication>

#include <filesystem>

class Config {
public:
    enum class Command {
        COPY,
        MOVE,
        COPY_JPEG,
        MOVE_JPEG,
        SIMULATE
    };

    enum class UI {
        FullGui,
        SlimGui,
        NoGui
    };

    Config(const QApplication& app);

    bool readConfigFile() noexcept;
    bool writeConfigFile() const noexcept;

    void setCommand(const Command& command);
    bool setCommand(const QString& command);
    void setPattern(const QString& pattern);
    void setInputDir(const QString& inputDir);
    void setOutputDir(const QString& outputDir);

    static const QString commandString(const Command& command);

    const Command& command() const { return m_command; }
    const UI& ui() const { return m_ui; }
    const std::string& pattern() const { return m_pattern; }
    const std::filesystem::path& inputDir() const { return m_inputDir; }
    const std::filesystem::path& outputDir() const { return m_outputDir; }

private:
    Command m_command = Command::COPY_JPEG;
    UI m_ui = UI::FullGui;
    std::string m_pattern = "%Y/%W/IMG_%Y%m%d_%H%M%S";
    std::filesystem::path m_inputDir;
    std::filesystem::path m_outputDir;
};
