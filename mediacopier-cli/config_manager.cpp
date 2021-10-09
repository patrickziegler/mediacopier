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

#include "config_manager.hpp"

#include <mediacopier/version.hpp>

#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace mediacopier::cli {

ConfigManager::ConfigManager(int argc, char *argv[])
{
    std::vector<std::string> posArgs;

    for (int i = 1; i < argc; ++i) {
        std::string arg{argv[i]};

        if (!arg.compare(0, 1, "-")) {

            if (arg.find("h") < arg.npos) {
                posArgs.clear();
                break;
            }

            if (arg.find("f") < arg.npos) {
                if (i + 1 < argc) {
                    ++i;
                    pattern = std::string{argv[i]};
                } else {
                    std::cerr << "Option '-f' requires an argument" << std::endl;
                    std::exit(1);
                }
            }

        } else {
            posArgs.push_back(arg);
        }
    }

    if (posArgs.size() < 3) {
        std::cout << MEDIACOPIER_PROJECT_NAME << " v" << MEDIACOPIER_VERSION
                  << ", Copyright (C) 2020-2021 Patrick Ziegler\n\n"
                  << "Usage: mediacopier COMMAND SRC DST [options]\n"
                  << "\nAvailable commands:\n"
                  << "  copy   : Copy files from SRC directory to DST\n"
                  << "  move   : The same as 'copy', but removing successfully copied files\n"
                  << "  sim    : Simulate operation, show source and destination paths\n"
                  << "\nOptions and arguments:\n"
                  << "  -h     : Show this help message and exit\n"
                  << "  -f FMT : Set the base format to be used for new filenames (default: " << pattern << ")\n"
                  << "           as defined in https://howardhinnant.github.io/date/date.html#to_stream_formatting\n\n";
        std::exit(0);
    }

    std::string op{posArgs.at(0)};

    if (posArgs.size() > 1) {
        inputDir = fs::path{posArgs.at(1)};
    }

    if (posArgs.size() > 2) {
        outputDir = fs::path{posArgs.at(2)};
    }

    if (op == "copy") {
        command = Command::COPY_JPEG;
    } else if (op == "move") {
        command = Command::MOVE_JPEG;
    } else if (op == "sim") {
        command = Command::SIMULATE;
    } else {
        std::cerr << "Unknown operation '" << op << "'" << std::endl;
        std::exit(1);
    }
}

} // namespace mediacopier::cli
