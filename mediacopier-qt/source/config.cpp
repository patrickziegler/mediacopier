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

static constexpr const char* MEDIACOPIER_CONFIG_FILE = ".mediacopier";

static const std::map<QString, Config::Command> commands = {
    {"copy", Config::Command::Copy},
    {"move", Config::Command::Move}
};

Config::Config(const QApplication& app)
{
    QCommandLineParser parser;

    parser.setApplicationDescription(
                app.applicationName() +
                ", Copyright (C) 2020-2025 Patrick Ziegler");
    parser.addPositionalArgument(
                "CMD", "Available commands: copy (default), move", "[CMD");
    parser.addPositionalArgument(
                "SRC", "Input directory", "[SRC");
    parser.addPositionalArgument(
                "DST", "Output directory", "[DST]]]");
    QCommandLineOption optSlimGui(
                "slim-gui", "Launch simplified user interface");

    parser.addOptions({optSlimGui});
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
}

bool Config::readConfigFile() noexcept
{
    const auto configFile = m_outputDir / MEDIACOPIER_CONFIG_FILE;
    if (!fs::is_regular_file(configFile)) {
        return false;
    }
    const toml::value input = toml::parse(configFile);
    if(input.contains("pattern") && input.at("pattern").is_string()) {
        m_pattern = input.at("pattern").as_string();
    }
    if(input.contains("useUtc") && input.at("useUtc").is_boolean()) {
        if (input.at("useUtc").as_boolean()) {
            m_timezone = Timezone::Universal;
        } else {
            m_timezone = Timezone::Local;
        }
    }
    return true;
}

bool Config::writeConfigFile() const noexcept
{
    if (!fs::is_directory(m_outputDir)) {
        return false;
    }
    const auto configFile = m_outputDir / MEDIACOPIER_CONFIG_FILE;
    toml::value output{{"pattern", m_pattern.get()}, {"useUtc", useUtc()}};
    std::ofstream os{configFile};
    os << "# this file is updated on every run of mediacopier"
       << ", manual changes might be lost\n" << output;
    return true;
}

void Config::setInputDir(const QString& inputDir)
{
    m_inputDir = inputDir.toStdString();
}

void Config::setOutputDir(const QString& outputDir)
{
    m_outputDir = outputDir.toStdString();
}

void Config::setPattern(const QString& pattern)
{
    m_pattern = pattern.toStdString();
}

void Config::setTimezone(const Timezone& timezone)
{
    m_timezone = timezone;
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

void Config::resetPattern()
{
    m_pattern.reset();
}

void Config::resetTimezone()
{
    m_timezone.reset();
}

bool Config::useUtc() const
{
    return m_timezone == Timezone::Universal;
}
