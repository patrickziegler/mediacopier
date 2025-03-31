/* Copyright (C) 2022 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/persistent_config.hpp>

#include <QApplication>

class Config : public mediacopier::PersistentConfig {
public:
    enum class Command {
        Copy,
        Move
    };
    enum class Timezone {
        Universal,
        Local
    };

    Config(const QApplication& app);

    bool readConfigFile() noexcept;
    bool writeConfigFile() const noexcept;

    void setInputDir(const QString& inputDir);
    void setOutputDir(const QString& outputDir);
    void setPattern(const QString& pattern);
    void setTimezone(const Timezone& timezone);
    void setCommand(const Command& command);
    void setCommand(const QString& command);

    auto getInputDir() const -> const std::filesystem::path&;
    auto getOutputDir() const -> const std::filesystem::path&;
    auto getPattern() const -> const std::string&;
    auto getTimezone() const -> const Timezone;
    auto getCommand() const -> const Command;

    void resetPattern();
    void resetTimezone();

    bool useUtc() const;

private:
    std::filesystem::path m_inputDir;
    std::filesystem::path m_outputDir;
    Command m_command = Command::Copy;
};
