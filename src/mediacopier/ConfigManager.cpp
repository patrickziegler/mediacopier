// Copyright (C) 2019 Patrick Ziegler
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "ConfigManager.hpp"
#include <iostream>

namespace bf = boost::filesystem;

ConfigManager* ConfigManager::pInstance_ = nullptr;

ConfigManager& ConfigManager::instance()
{
    if (!pInstance_) {
        if (!pInstance_) {
            pInstance_ = new ConfigManager;
        }
    }
    return *pInstance_;
}

int ConfigManager::parseArgs(int argc, char *argv[])
{
    std::string arg;
    std::vector<std::string> pos;

    for (int i = 1; i < argc; ++i) {
        arg = std::string(argv[i]);

        if (!arg.compare(0, 1, "-")) {

            if (arg.find("h") < arg.npos) {
                pos.clear();
                break;
            }

            if (arg.find("o") < arg.npos) {
                flagOverride = true;
            }

            if (arg.find("s") < arg.npos) {
                flagSimulate = true;
            }

            if (arg.find("f") < arg.npos) {

                if (i + 1 < argc) {
                    pathFormat = std::string(argv[++i]);
                } else {
                    std::cerr << "Option '-f' requires an argument" << std::endl;
                    return 1;
                }
            }

            if (arg.find("l") < arg.npos) {

                if (i + 1 < argc) {
                    logfile = std::string(argv[++i]);
                } else {
                    std::cerr << "Option '-l' requires an argument" << std::endl;
                    return 1;
                }
            }

        } else {
            pos.push_back(arg);
        }
    }

    if (pos.size() < 2) {
        std::cout << "MediaCopier, Copyright (C) 2019 Patrick Ziegler" <<  std::endl << std::endl
                  << "Usage: [-h] [-o] [-s] [-f FORMAT] [-l LOGFILE] SOURCE DESTINATION" << std::endl;

        return 1;
    }

    dirInput = bf::path(pos[0]);
    dirOutput = bf::path(pos[1]);

    return 0;
}
