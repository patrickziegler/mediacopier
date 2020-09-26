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

#include <mediacopier/version.hpp>

#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace mediacopier::cli {

class Config {
public:
    enum class Command {
        UNKNOWN,
        COPY,
        MOVE,
        GUI,
    };

    void parseArgs(int argc, char *argv[])
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
                        m_baseFormat = std::string(argv[++i]);
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
            std::cout << "MediaCopier v" << MEDIACOPIER_VERSION << ", Copyright (C) 2020 Patrick Ziegler\n"
                      << "Usage: mediacopier [options] COMMAND\n"
                      << "\nAvailable commands:\n"
                      << "  copy SRC DST : Copy files from SRC directory to DST\n"
                      << "  move SRC DST : The same as 'copy', but removing successfully copied files\n";
#ifdef ENABLE_GUI
            std::cout << "  gui          : Show graphical user interface\n";
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

    void setInputDir(fs::path dir) {
        m_inputDir = std::move(dir);
    }

    void setOutputDir(fs::path dir) {
        m_outputDir = std::move(dir);
    }

    void setBaseFormat(std::string fmt) {
        m_outputDir = std::move(fmt);
    }

    void setCommand(Command op) {
        m_command = op;
#ifndef ENABLE_GUI
        if (op == Command::GUI) {
            m_command = Command::UNKNOWN;
        }
#endif
    }

    fs::path inputDir() const
    {
        return m_inputDir;
    }

    fs::path outputDir() const
    {
        return m_outputDir;
    }

    std::string baseFormat() const
    {
        return m_baseFormat;
    }

    Command command() const
    {
        return m_command;
    }

private:
    fs::path m_inputDir;
    fs::path m_outputDir;
    std::string m_baseFormat = "%Y/%m/%d/IMG_%Y%m%d_%H%M%S_";
    Command m_command = Command::UNKNOWN;
};

}
