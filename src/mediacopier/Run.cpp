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
#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"
#include "ProgressBar.hpp"
#include "Run.hpp"
#include <chrono>
#include <iostream>
#include <thread>

using strategy_ptr = std::shared_ptr<FileOperationStrategy>;
using directory_iter = boost::filesystem::recursive_directory_iterator;
using timer = std::chrono::high_resolution_clock;

float get_duration(timer::time_point tp)
{
    std::chrono::duration<float> dt = timer::now() - tp;
    return dt.count();
}

int run(int opType, int argc, char *argv[])
{
    std::vector<boost::filesystem::path> files;

    if (ConfigManager::instance().parseArgs(argc, argv)) {
        return 1;
    }

    std::cout << "Input dir:\t" << ConfigManager::instance().dirInput.string() << std::endl
              << "Output dir:\t" << ConfigManager::instance().dirOutput.string() << std::endl << std::endl
              << "Searching for media files ...";

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

    FileOperation::setStrategy(strategy);
    FileOperation::setPathFormat(ConfigManager::instance().pathFormat);
    FileOperation::setPathPrefix(ConfigManager::instance().dirOutput);

    ProgressBar* bar = new ProgressBar(files.size(), strategy->description);
    timer::time_point tp = timer::now();

    if (ConfigManager::instance().logfile != "") {

        std::ofstream log;
        log.open(ConfigManager::instance().logfile, std::ios::out | std::ios::trunc);

#pragma omp parallel sections
        {

#pragma omp section
            {
                while (!bar->hasFinished()) {
                    bar->showProgress();
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                bar->close();
            }

#pragma omp section
            {

#pragma omp parallel for shared(files)
                for (size_t i = 0; i < files.size(); ++i) {

                    std::ostringstream message;

                    try {

                        FileOperation op(files[i]);

                        message << strategy->getLogMessage(op.execute()) << std::endl
                                << op.getPathOld() << std::endl
                                << op.getPathNew() << std::endl
                                << std::endl;

                    } catch (const std::invalid_argument& e) {

                        message << "File operation [" << strategy->description << "] SKIPPED: " << e.what() << std::endl
                                << files[i].string() << std::endl
                                << std::endl;
                    }

#pragma omp critical
                    {
                        log << message.str();
                        ++(bar->i);
                    }

                }

                log.close();

            }

        }

    } else {

#pragma omp parallel sections
        {

#pragma omp section
            {
                while (!bar->hasFinished()) {
                    bar->showProgress();
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                bar->close();
            }

#pragma omp section
            {

#pragma omp parallel for shared(files)
                for (size_t i = 0; i < files.size(); ++i) {

                    try {
                        FileOperation op(files[i]);
                        op.execute();

                    } catch (const std::invalid_argument&) {

                    }
#pragma omp atomic
                    ++(bar->i);
                }

            }

        }

    }

    std::cout << std::endl << "Operation took " << get_duration(tp) << " s" << std::endl;

    return 0;
}
