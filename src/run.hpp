#ifndef RUN_H
#define RUN_H

#include "ConfigManager.hpp"
#include "FileManager.hpp"
#include "ProgressBar.hpp"

#include <iostream>
#include <chrono>
#include <vector>

template<opType op>
std::string get_log_message(const FileManager& file, const int& result)
{
    std::ostringstream out;

    switch (op)
    {
    case Copy:
        out << "[copying]";
        break;
    case Move:
        out << "[moving]";
        break;
    case Simulate:
        out << "[simulation]";
        break;
    default:
        out << "[nop]";
        break;
    }

    switch (result)
    {
    case 0:
        out << " OK";
        break;
    case 1:
        out << " FAILED: Could not override file";
        break;
    case 2:
        out << " FAILED: File already exists";
        break;
    case 3:
        out << " FAILED: File could not be deleted";
        break;
    case 4:
        out << " FAILED: Could not create directories";
        break;
    default:
        out << " FAILED: Unknown error number";
        break;
    }

    out << std::endl
        << file.getPathOld() << std::endl
        << file.getPathNew() << std::endl
        << std::endl;

    return out.str();
}

template<opType op>
int run(int argc, char *argv[])
{
    ProgressBar* bar;
    std::ofstream log;
    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::duration<float> dt;

    if (ConfigManager::instance().parseArgs(argc, argv)) {
        return 1;
    }

    if (ConfigManager::instance().logfile != "") {
        log.open(ConfigManager::instance().logfile, std::ios::out | std::ios::trunc);
    }

    std::cout << "Input dir:\t" << ConfigManager::instance().dirInput.string() << std::endl
              << "Output dir:\t" << ConfigManager::instance().dirOutput.string() << std::endl << std::endl
              << "Searching for media files ..." << std::endl << std::endl;

    std::vector<FileManager> files;

    t0 = std::chrono::high_resolution_clock::now();

    for (auto i = boost::filesystem::recursive_directory_iterator(ConfigManager::instance().dirInput); i != boost::filesystem::recursive_directory_iterator(); ++i) {
        try {
            if (!is_directory(i->path())) {
                std::cout << "Found " << i->path().string() << ": ";
                files.push_back(FileManager(i->path()));
                std::cout << "OK" << std::endl;
            }
        } catch (const std::invalid_argument&) {
            std::cout << "SKIPPED" << std::endl;
            continue;
        }
    }

    dt = std::chrono::high_resolution_clock::now() - t0;
    
    std::cout << std::endl
              << "-> " << files.size()
              << " valid files (" << dt.count() << " s)" << std::endl << std::endl;

    t0 = std::chrono::high_resolution_clock::now();

    if (ConfigManager::instance().flagSimulate || op == Simulate) {

        bar = new ProgressBar(files.size(), "Simulation");
        std::vector<std::string> filesDone;
        std::string path;
        int result;

        for (size_t i = 0; i < files.size(); ++i) {

            if (!(result = files[i].simulate())) {
                path = files[i].getPathNew();

                if (ConfigManager::instance().flagOverride || (std::find(filesDone.begin(), filesDone.end(), path) == filesDone.end())) {
                    filesDone.push_back(path);
                } else {
                    result = 2;
                }
            }

            if (ConfigManager::instance().logfile != "") {
                log << get_log_message<Simulate>(files[i], result);
            }
            bar->update();
        }

    } else {

        switch (op)
        {
        case Copy:
            bar = new ProgressBar(files.size(), "Copying files");
            break;
        case Move:
            bar = new ProgressBar(files.size(), "Moving files");
            break;
        default:
            bar = new ProgressBar(files.size(), "Nop");
            break;
        }

        if (ConfigManager::instance().logfile != "") {
#pragma omp parallel for shared(files, bar)
            for (size_t i = 0; i < files.size(); ++i) {
                std::string message = get_log_message<op>(files[i], files[i].operation<op>());
                bar->update();
#pragma omp critical
                {
                    log << message;
                }
            }
        } else {
#pragma omp parallel for shared(files, bar)
            for (size_t i = 0; i < files.size(); ++i) {
                files[i].operation<op>();
                bar->update();
            }
        }
    }

    log.close();

    dt = std::chrono::high_resolution_clock::now() - t0;
    std::cout << std::endl << "Operation took " << dt.count() << " s" << std::endl;
    return 0;
}

#endif
