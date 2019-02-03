#ifndef RUN_H
#define RUN_H

#include "ConfigManager.hpp"
#include "FileManager.hpp"
#include "ProgressBar.hpp"

#include <iostream>
#include <vector>

template<opType op>
int run(int argc, char *argv[])
{
    ProgressBar* bar;

    if (ConfigManager::instance().parseArgs(argc, argv)) {
        return 1;
    }

    std::cout << "Input dir:\t" << ConfigManager::instance().dirInput.string() << std::endl
              << "Output dir:\t" << ConfigManager::instance().dirOutput.string() << std::endl
              << std::endl
              << "Searching for media files ...";

    std::vector<FileManager> files;

    for (auto i = boost::filesystem::recursive_directory_iterator(ConfigManager::instance().dirInput); i != boost::filesystem::recursive_directory_iterator(); ++i) {
        try {
            if (!is_directory(i->path())) {
                files.push_back(FileManager(i->path()));
            }
        } catch (const std::invalid_argument&) {
            continue;
        }
    }

    std::cout << " Done (" << files.size() << " files found)" << std::endl << std::endl;

    if (ConfigManager::instance().flagSimulate || op == Simulate) {

        bar = new ProgressBar(files.size(), "Simulation");
        std::vector<std::string> filesDone;
        std::string path;

        for (size_t i = 0; i < files.size(); ++i) {

            if (!files[i].simulate()) {
                path = files[i].getPathNew();

                if (ConfigManager::instance().flagOverride || (std::find(filesDone.begin(), filesDone.end(), path) == filesDone.end())) {
                    filesDone.push_back(path);
                    // std::cout << path << std::endl;
                }
            }
            bar->update();
        }

    } else {

        if (op == Move) {
            bar = new ProgressBar(files.size(), "Moving files");
        } else {
            bar = new ProgressBar(files.size(), "Copying files");
        }

#pragma omp parallel for shared(files, bar)
        for (size_t i = 0; i < files.size(); ++i) {
            if (!files[i].operation<op>()) {
                // std::cout << files[i].getPathNew() << std::endl;
            }
            bar->update();
        }
    }

    std::cout << std::endl << "Operation finished successfully!" << std::endl;
    return 0;
}

#endif
