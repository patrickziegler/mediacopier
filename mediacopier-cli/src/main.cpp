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

#include <mediacopier/Executor.hpp>
#include <mediacopier/version.hpp>

#include <log4cplus/log4cplus.h>

#include <iostream>

namespace fs = std::filesystem;

namespace MediaCopier {

Executor createExecutor(int argc, char *argv[])
{
    std::vector<std::string> pos;

    fs::path inputDir;
    fs::path outputDir;
    std::string pattern = "%Y/%m/%d/IMG_%Y%m%d_%H%M%S_";
    Executor::Command command;

    for (int i = 1; i < argc; ++i) {
        std::string arg{argv[i]};

        if (!arg.compare(0, 1, "-")) {

            if (arg.find("h") < arg.npos) {
                pos.clear();
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
            pos.push_back(arg);
        }
    }

    if (pos.size() < 3) {
        std::cout << MEDIACOPIER_PROJECT_NAME << " v" << MEDIACOPIER_VERSION
                  << ", Copyright (C) 2020 Patrick Ziegler\n"
                  << "Usage: mediacopier COMMAND SRC DST [options]\n"
                  << "\nAvailable commands:\n"
                  << "  copy   : Copy files from SRC directory to DST\n"
                  << "  move   : The same as 'copy', but removing successfully copied files\n"
                  << "\nOptions and arguments:\n"
                  << "  -h     : Show this help message and exit\n"
                  << "  -f FMT : Set the base format to be used for new filenames\n"
                  << "           (default: " << pattern << ")\n";
        std::exit(0);
    }

    std::string op{pos.at(0)};

    if (pos.size() > 1) {
        inputDir = fs::path{pos.at(1)};
    }

    if (pos.size() > 2) {
        outputDir = fs::path{pos.at(2)};
    }

    if (op == "copy") {
        command = Executor::Command::COPY;
    } else if (op == "move") {
        command = Executor::Command::MOVE;
    } else {
        std::cerr << "Unknown operation '" << op << "'" << std::endl;
        std::exit(1);
    }

    return {command, inputDir, outputDir, pattern};
}

} // namespace MediaCopier

int main(int argc, char *argv[])
{
    log4cplus::BasicConfigurator log;
    log.configure();

    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    auto executor = MediaCopier::createExecutor(argc, argv);

    try {
        executor.run();
    } catch (const std::exception& err) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(err.what()));
        return 1;
    }

    return 0;
}
