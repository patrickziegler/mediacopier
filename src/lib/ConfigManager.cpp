#include "ConfigManager.hpp"
#include <iostream>

namespace bf = boost::filesystem;

ConfigManager* ConfigManager::pInstance_ = nullptr;
std::mutex ConfigManager::mtx;

ConfigManager& ConfigManager::instance()
{
    if (!pInstance_) {
        std::lock_guard<std::mutex> lck(mtx);
        if (!pInstance_) {
            pInstance_ = new ConfigManager;
        }
    }
    return *pInstance_;
}

int ConfigManager::parseArgs(int argc, char *argv[])
{
    std::lock_guard<std::mutex> lck(mtx);

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
                    format = std::string(argv[++i]);
                } else {
                    std::cerr << "Option '-f' requires an argument" << std::endl;
                    return 1;
                }
            }

        } else {
            pos.push_back(arg);
        }
    }

    if (pos.size() < 2) {
        std::cout << "Usage: [-h] [-o] [-s] [-f FORMAT] SOURCE DESTINATION" << std::endl;
        return 1;
    }

    dirInput = bf::path(pos[0]);
    dirOutput = bf::path(pos[1]);

    return 0;
}
