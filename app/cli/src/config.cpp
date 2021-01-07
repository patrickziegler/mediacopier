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

#include <mediacopier/cli/config.hpp>
#include <mediacopier/version.hpp>

#include <iostream>
#include <vector>

namespace cli = MediaCopier::Cli;
namespace fs = std::filesystem;

void cli::ConfigManager::parseArgs(int argc, char *argv[])
{
    std::vector<std::string> pos;

    for (int i = 1; i < argc; ++i) {
        std::string arg{argv[i]};

        if (!arg.compare(0, 1, "-")) {

            if (arg.find("h") < arg.npos) {
                pos.clear();
                break;
            }

            if (arg.find("f") < arg.npos) {
                if (i + 1 < argc) {
                    m_baseFormat = std::string{argv[++i]};
                } else {
                    std::cerr << "Option '-f' requires an argument" << std::endl;
                    std::exit(1);
                }
            }

        } else {
            pos.push_back(arg);
        }
    }

    if (pos.size() < 1) {
        std::cout << MEDIACOPIER_PROJECT_NAME << " v" << MEDIACOPIER_VERSION
                  << ", Copyright (C) 2020 Patrick Ziegler\n"
                  << "Usage: mediacopier COMMAND SRC DST [options]\n"
                  << "\nAvailable commands:\n"
                  << "  copy : Copy files from SRC directory to DST\n"
                  << "  move : The same as 'copy', but removing successfully copied files\n";
#ifdef ENABLE_GUI
        std::cout << "  gui  : Show graphical user interface\n";
#endif
        std::cout << "\nOptions and arguments:\n"
                  << "  -h     : Show this help message and exit\n"
                  << "  -f FMT : Set the base format to be used for new filenames\n"
                  << "           (default: " << m_baseFormat << ")\n";
        std::exit(0);
    }

    std::string op{pos.at(0)};

    if (pos.size() > 1) {
        m_inputDir = fs::path{pos.at(1)};
    }

    if (pos.size() > 2) {
        m_outputDir = fs::path{pos.at(2)};
    }

#ifdef ENABLE_GUI
    if (op == "gui") {
        m_command = Command::GUI;
    }
#endif

    if (op == "copy") {
        m_command = Command::COPY;
    } else if (op == "move") {
        m_command = Command::MOVE;
    }

    if (m_command == Command::UNKNOWN) {
        std::cerr << "Unknown operation '" << op << "'" << std::endl;
        std::exit(1);
    }
}

void cli::ConfigManager::setInputDir(fs::path dir) {
    m_inputDir = std::move(dir);
}

void cli::ConfigManager::setOutputDir(fs::path dir) {
    m_outputDir = std::move(dir);
}

void cli::ConfigManager::setBaseFormat(std::string fmt) {
    m_outputDir = std::move(fmt);
}

void cli::ConfigManager::setCommand(Command op) {
    m_command = op;
#ifndef ENABLE_GUI
    if (op == cli::Command::GUI) {
        m_command = cli::Command::UNKNOWN;
    }
#endif
}

fs::path cli::ConfigManager::inputDir() const
{
    return m_inputDir;
}

fs::path cli::ConfigManager::outputDir() const
{
    return m_outputDir;
}

std::string cli::ConfigManager::baseFormat() const
{
    return m_baseFormat;
}

cli::Command cli::ConfigManager::command() const
{
    return m_command;
}
