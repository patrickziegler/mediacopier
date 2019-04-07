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


#ifndef RUN_H
#define RUN_H

#include "ConfigManager.hpp"
#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"
#include "ProgressBar.hpp"
#include <iostream>
#include <chrono>

using operationType = enum {
Copy,
Move,
Simulate
};

using strategy_ptr = std::shared_ptr<FileOperationStrategy>;
using directory_iter = boost::filesystem::recursive_directory_iterator;
using timer = std::chrono::high_resolution_clock;

inline float get_duration(timer::time_point tp)
{
    std::chrono::duration<float> dt = timer::now() - tp;
    return dt.count();
}

template<operationType opType>
int run(int argc, char *argv[])
{
    ProgressBar* bar;
    std::ofstream log;
    timer::time_point tp;

    std::vector<boost::filesystem::path> files;

    if (ConfigManager::instance().parseArgs(argc, argv)) {
        return 1;
    }

    std::cout << "Input dir:\t" << ConfigManager::instance().dirInput.string() << std::endl
              << "Output dir:\t" << ConfigManager::instance().dirOutput.string() << std::endl << std::endl
              << "Searching for media files ...";

    tp = timer::now();

    for (auto i = directory_iter(ConfigManager::instance().dirInput); i != directory_iter(); ++i) {
        if (!is_directory(i->path())) {
            files.push_back(i->path());
        }
    }

    std::cout << " OK (" << files.size() << " files found)" << std::endl << std::endl;

    strategy_ptr strategy;

    if (ConfigManager::instance().flagSimulate || opType == Simulate) {

        if (ConfigManager::instance().flagOverride) {
            strategy = strategy_ptr(new FileSimulationOverwrite());
        } else {
            strategy = strategy_ptr(new FileSimulation());
        }

    } else if (opType == Move) {

        if (ConfigManager::instance().flagOverride) {
            strategy = strategy_ptr(new FileMoveOverwrite());
        } else {
            strategy = strategy_ptr(new FileMove());
        }

    } else {

        if (ConfigManager::instance().flagOverride) {
            strategy = strategy_ptr(new FileCopyOverwrite());
        } else {
            strategy = strategy_ptr(new FileCopy());
        }
    }

    bar = new ProgressBar(files.size(), strategy->description);

    FileOperation::setStrategy(strategy);
    FileOperation::setPathFormat(ConfigManager::instance().pathFormat);
    FileOperation::setPathPrefix(ConfigManager::instance().dirOutput);

    tp = timer::now();

    if (ConfigManager::instance().logfile != "") {

        log.open(ConfigManager::instance().logfile, std::ios::out | std::ios::trunc);

#pragma omp parallel for shared(files)
        for (size_t i = 0; i < files.size(); ++i) {
            std::string message;
            try {
                FileOperation op(files[i]);
                message = op.getLogMessage(op.execute());

            } catch (const std::invalid_argument& e) {
                std::ostringstream buf;
                buf << "File operation [" << strategy->description << "] SKIPPED: " << e.what() << std::endl
                    << files[i].string() << std::endl << std::endl;
                message = buf.str();
            }
#pragma omp critical
            {
                log << message;
                bar->update();
            }
        }

        log.close();

    } else {

#pragma omp parallel for shared(files)
        for (size_t i = 0; i < files.size(); ++i) {
            try {
                FileOperation op(files[i]);
                op.execute();
            } catch (const std::invalid_argument&) {}
#pragma omp critical
            {
                bar->update();
            }
        }
    }

    bar->close();
    std::cout << std::endl << "Operation took " << get_duration(tp) << " s" << std::endl;
    return 0;
}

#endif
