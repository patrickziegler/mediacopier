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

#include <filesystem>

struct Config
{
    enum class Command {
        Copy,
        Move
    };

    enum class ParseResult {
        Continue,
        Break
    };

    std::pair<ParseResult, int> parseArgs(int argc, char *argv[]);
    void loadPersistentConfig();
    void storePersistentConfig() const;
    void finalize() noexcept;

    Command cmd = Command::Copy;
    std::filesystem::path inputDir;
    std::filesystem::path outputDir;
    std::string pattern;
};
