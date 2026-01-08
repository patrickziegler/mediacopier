/* Copyright (C) 2023 Patrick Ziegler <zipat@proton.me>
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

namespace mediacopier {

class Cli : public PersistentConfig {
public:
    enum class Command {
        Copy,
        Move,
        Sim,
    };
    enum class ParseResult {
        Continue,
        Break
    };
    std::pair<ParseResult, int> parseArgs(int argc, char* argv[]);
    auto command() const -> Command { return m_command; }
    auto inputDir() const -> const std::filesystem::path& { return m_inputDir; }
    auto outputDir() const -> const std::filesystem::path& { return m_outputDir; }
    auto pattern() const -> const std::string& { return m_pattern.get(); }
    auto useUtc() const -> bool { return m_useUtc; }

private:
    Command m_command = Command::Copy;
    std::filesystem::path m_inputDir;
    std::filesystem::path m_outputDir;
};

} // namespace mediacopier
