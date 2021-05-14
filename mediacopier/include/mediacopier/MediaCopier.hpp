/* Copyright (C) 2021 Patrick Ziegler
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

namespace MediaCopier {

class MediaCopier {
public:
    enum class Command {
        COPY,
        MOVE,
    };
    MediaCopier(Command command, std::filesystem::path inputDir, std::filesystem::path outputDir, std::string pattern)
        : m_command{std::move(command)},
          m_inputDir{std::move(inputDir)},
          m_outputDir{std::move(outputDir)},
          m_pattern{std::move(pattern)} {}
    void run();
private:
    Command m_command;
    std::filesystem::path m_inputDir;
    std::filesystem::path m_outputDir;
    std::string m_pattern;
};

} // namespace MediaCopier
