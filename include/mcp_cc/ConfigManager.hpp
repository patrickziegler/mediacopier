#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <mutex>
#include <boost/filesystem.hpp>

class ConfigManager
{
    static ConfigManager* pInstance_;
    static std::mutex mtx;

    ConfigManager() {}
    ConfigManager(const ConfigManager&) {}
    ConfigManager& operator=(const ConfigManager&);
    ~ConfigManager() { pInstance_ = nullptr; }

public:
    std::string format = "%Y/%Y-%m/%Y-%m-%d/IMG_%Y%m%d_%H%M%S_%f";
    std::string logfile = "";
    bool flagOverride = false;
    bool flagSimulate = false;
    boost::filesystem::path dirInput;
    boost::filesystem::path dirOutput;

    static ConfigManager& instance();
    int parseArgs(int argc, char *argv[]);
};

#endif
