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

#include "config.hpp"

#include <QCommandLineParser>
#include <QSettings>

namespace fs = std::filesystem;

static constexpr const char* CONFIG_FILE = ".mediacopier";
static constexpr const char* CONFIG_KEY_PATTERN = "Core/pattern";

Config::Config(const QApplication& app)
{
    QCommandLineParser parser;

    parser.setApplicationDescription(
                app.applicationName() +
                ", Copyright (C) 2020-2022 Patrick Ziegler");

    parser.addPositionalArgument(
                "SRC", "Input directory", "[SRC");

    parser.addPositionalArgument(
                "DST", "Output directory", "[DST]]");

    QCommandLineOption optCommand(
                "c", "Available commands: copy (default), move, show",
                "command");

    QCommandLineOption optPattern(
                "f", "Pattern to be used for creating new filenames",
                "pattern");

    parser.addOptions({optCommand, optPattern});
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    if (parser.positionalArguments().length() > 0)
        setInputDir(parser.positionalArguments().at(0));

    if (parser.positionalArguments().length() > 1)
        setOutputDir(parser.positionalArguments().at(1));

    readConfigFile();

    if (parser.isSet("f"))
        setPattern(parser.value("f"));

    if (parser.isSet("c")) {
        const auto cmd = parser.value("c");
        if (!setCommand(cmd))
            throw std::runtime_error("No such command '" + cmd.toStdString() + "'");
    }
}

bool Config::readConfigFile() noexcept
{
    bool result = false;
    const auto file = m_outputDir / CONFIG_FILE;
    if (fs::is_regular_file(file)) {
        QSettings settings{QString::fromStdString(file.string()), QSettings::IniFormat};
        if (settings.value(CONFIG_KEY_PATTERN).isValid()) {
            m_pattern = settings.value(CONFIG_KEY_PATTERN).toString().toStdString();
            result = true;
        }
    }
    return result;
}

bool Config::writeConfigFile() const noexcept
{
    bool result = false;
    if (fs::is_directory(m_outputDir)) {
        const auto file = m_outputDir / CONFIG_FILE;
        QSettings settings{QString::fromStdString(file.string()), QSettings::IniFormat};
        settings.setValue(CONFIG_KEY_PATTERN, QString::fromStdString(m_pattern));
        settings.sync();
        result = true;
    }
    return result;
}

void Config::setCommand(const Command& command)
{
    m_command = command;
}

bool Config::setCommand(const QString& command)
{
    auto cmd = command.toLower();

    if (cmd == "copy")
        m_command = Command::COPY_JPEG;
    else if (cmd == "move")
        m_command = Command::MOVE_JPEG;
    else if (cmd == "show")
        m_command = Command::SIMULATE;
    else
        return false;

    return true;
}

void Config::setPattern(const QString& pattern)
{
    m_pattern = pattern.toStdString();
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
    switch(command) {

    case Config::Command::COPY:
        return QObject::tr("Copy");

    case Config::Command::COPY_JPEG:
        return QObject::tr("Copy");

    case Config::Command::MOVE:
        return QObject::tr("Move");

    case Config::Command::MOVE_JPEG:
        return QObject::tr("Move");

    case Config::Command::SIMULATE:
        return QObject::tr("Simulate");

    default:
        return QObject::tr("Unknown");
    }
}
