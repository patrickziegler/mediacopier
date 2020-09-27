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

#include <filesystem>

namespace MediaCopier::CLI {

class ConfigStore {
public:
    enum class Command {
        UNKNOWN,
        COPY,
        MOVE,
        GUI,
    };

    void parseArgs(int argc, char *argv[]);

    void setInputDir(std::filesystem::path dir);
    void setOutputDir(std::filesystem::path dir);
    void setBaseFormat(std::string fmt);
    void setCommand(Command op);

    std::filesystem::path inputDir() const;
    std::filesystem::path outputDir() const;
    std::string baseFormat() const;
    Command command() const;

private:
    std::filesystem::path m_inputDir;
    std::filesystem::path m_outputDir;
    std::string m_baseFormat = "%Y/%m/%d/IMG_%Y%m%d_%H%M%S_";
    Command m_command = Command::UNKNOWN;
};

}
