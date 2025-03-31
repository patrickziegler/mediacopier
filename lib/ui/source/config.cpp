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
}

bool Config::readConfigFile() noexcept
{
    m_pattern.resetDefault();
    m_useUtc.resetDefault();
    try {
        loadPersistentConfig(m_outputDir);
    } catch(...) {
        return false;
    }
    return true;
}

bool Config::writeConfigFile() const noexcept
{
    storePersistentConfig(m_outputDir);
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
    m_useUtc = (timezone == Timezone::Universal);
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

auto Config::getInputDir() const -> const std::filesystem::path&
{
    return m_inputDir;
}

auto Config::getOutputDir() const -> const std::filesystem::path&
{
    return m_outputDir;
}

auto Config::getPattern() const -> const std::string&
{
    return m_pattern.get();
}

auto Config::getTimezone() const -> const Timezone
{
    return (m_useUtc) ? Timezone::Universal : Timezone::Local;
}

auto Config::getCommand() const -> const Command {
    return m_command;
}

void Config::resetPattern()
{
    m_pattern.reset();
}

void Config::resetTimezone()
{
    m_useUtc.reset();
}

bool Config::useUtc() const
{
    return m_useUtc;
}
