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


#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <boost/filesystem.hpp>

class ConfigManager
{
    static ConfigManager* pInstance_;

    ConfigManager() {}
    ConfigManager(const ConfigManager&) {}
    ConfigManager& operator=(const ConfigManager&);
    ~ConfigManager() { pInstance_ = nullptr; }

public:
    static ConfigManager& instance();

    std::string pathFormat = "%Y/%Y-%m/%Y-%m-%d/IMG_%Y%m%d_%H%M%S_%f";
    std::string logfile = "";
    bool flagOverride = false;
    bool flagSimulate = false;
    boost::filesystem::path dirInput;
    boost::filesystem::path dirOutput;

    int parseArgs(int argc, char *argv[]);
};

#endif
