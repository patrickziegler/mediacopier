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

#include "config.hpp"

#include <QCommandLineParser>

#include <spdlog/spdlog.h>
#include <toml.hpp>

namespace fs = std::filesystem;

static constexpr const char* CONFIG_FILE = ".mediacopier";
static constexpr const char* DEFAULT_PATTERN = "%Y/%W/IMG_%Y%m%d_%H%M%S";

static const std::map<QString, Config::Command> commands = {
    {"copy", Config::Command::COPY},
    {"move", Config::Command::MOVE}
};

static const std::map<Config::Command, QString> commandStrings = {
    {Config::Command::COPY, QT_TRANSLATE_NOOP("Command", "Copy")},
    {Config::Command::MOVE, QT_TRANSLATE_NOOP("Command", "Move")}
};

Config::Config(const QApplication& app)
{
    QCommandLineParser parser;

    parser.setApplicationDescription(
                app.applicationName() +
                ", Copyright (C) 2020-2024 Patrick Ziegler");
    parser.addPositionalArgument(
                "CMD", "Available commands: copy (default), move", "[CMD");
    parser.addPositionalArgument(
                "SRC", "Input directory", "[SRC");
    parser.addPositionalArgument(
                "DST", "Output directory", "[DST]]]");
    QCommandLineOption optSlimGui(
                "slim-gui", "Pattern to be used for creating new filenames");
    QCommandLineOption optNoGui(
                "no-gui", "Pattern to be used for creating new filenames");

    parser.addOptions({optSlimGui, optNoGui});
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    if (parser.positionalArguments().length() > 0) {
        setCommand(parser.positionalArguments().at(0));
    }
    if (parser.positionalArguments().length() > 1) {
        setInputDir(parser.positionalArguments().at(1));
    }
    if (parser.positionalArguments().length() > 2) {
        setOutputDir(parser.positionalArguments().at(2));
        readConfigFile();
    }
    if (parser.isSet("slim-gui")) {
        m_guiType = GuiType::Slim;
    }

    m_pattern = DEFAULT_PATTERN;
}

bool Config::readConfigFile() noexcept
{
    const auto configFile = m_outputDir / CONFIG_FILE;
    if (!fs::is_regular_file(configFile)) {
        return false;
    }
    try {
        const auto data = toml::parse(configFile);
        m_pattern = toml::find<std::string>(data, "pattern");
    } catch (const std::exception& err) {
        spdlog::error("Failed to load config: " + std::string{err.what()});
        return false;
    }
    return true;
}

bool Config::writeConfigFile() const noexcept
{
    if (!fs::is_directory(m_outputDir)) {
        return false;
    }
    auto configFile = m_outputDir / CONFIG_FILE;
    toml::value data{{"pattern", m_pattern}};
    std::ofstream out{configFile};
    out << "# this file is updated on every run of mediacopier"
        << ", manual changes might be lost\n" << data;
    return true;
}

void Config::setCommand(const Command& command)
{
    m_command = command;
}

void Config::setCommand(const QString& command)
{
    try {
        m_command = commands.at(command.toLower());
    } catch (const std::out_of_range&) {
        throw std::runtime_error("No such command '" + command.toStdString() + "'");
    }
}

void Config::setPattern(const QString& pattern)
{
    m_pattern = pattern.toStdString();
}

void Config::resetPattern()
{
    m_pattern = DEFAULT_PATTERN;
}

void Config::setInputDir(const QString& inputDir)
{
    m_inputDir = inputDir.toStdString();
}

void Config::setOutputDir(const QString& outputDir)
{
    m_outputDir = outputDir.toStdString();
}

const QString Config::commandString(const Command& command)
{
    return QApplication::translate("Command", commandStrings.at(command).toStdString().c_str());
}
