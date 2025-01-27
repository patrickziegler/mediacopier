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

#include <QApplication>

#include <filesystem>
#include <optional>

constexpr const char* DEFAULT_PATTERN = "%Y/%W/IMG_%Y%m%d_%H%M%S";

template <typename T>
class Configurable {
public:
    Configurable(const T& defaultValue) :
        defaultValue{defaultValue},
        currentValue{std::nullopt} {}
    Configurable& operator=(const T& value) {
        currentValue = value;
        return *this;
    }
    T get() const {
        return currentValue.value_or(defaultValue);
    }
    operator T() const {
        return currentValue.value_or(defaultValue);
    }
    void set(const T& value) {
        currentValue = value;
    }
    void reset() {
        currentValue.reset();
    }
    void setDefault(const T& value) {
        defaultValue = value;
    }
private:
    T defaultValue;
    std::optional<T> currentValue;
};

class Config {
public:
    enum class Command {
        Copy,
        Move
    };
    enum class Timezone {
        Universal,
        Local
    };
    enum class GuiType {
        Full,
        Slim
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

    void resetPattern();
    void resetTimezone();

    bool useUtc() const;

    const GuiType& guiType() const { return m_guiType; }
    const std::filesystem::path& inputDir() const { return m_inputDir; }
    const std::filesystem::path& outputDir() const { return m_outputDir; }
    const std::string pattern() const { return m_pattern; }
    const Timezone timezone() const {return m_timezone; }
    const Command& command() const { return m_command; }

private:  
    GuiType m_guiType = GuiType::Full;
    std::filesystem::path m_inputDir;
    std::filesystem::path m_outputDir;
    Configurable<std::string> m_pattern{DEFAULT_PATTERN};
    Configurable<Timezone> m_timezone{Timezone::Universal};
    Command m_command = Command::Copy;
};
