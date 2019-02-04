#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

using opType = enum {
Copy,
Move,
Simulate
};

class FileManager
{
    bool flagOverride;
    boost::filesystem::path pathOld;
    boost::filesystem::path pathNew;
    boost::posix_time::ptime timestamp;

    static std::locale outputFormat;
    static boost::posix_time::ptime readExif(const boost::filesystem::path&);
    static boost::posix_time::ptime readVideoMeta(const boost::filesystem::path&);

public:
    FileManager(const boost::filesystem::path&);
    std::string getPathOld() const;
    std::string getPathNew() const;
    int copy();
    int move();
    int simulate();

    template<opType op>
    int operation() {
        switch (op)
        {
        case Copy:
            return copy();
        case Move:
            return move();
        case Simulate:
            return simulate();
        default:
            return 0;
        }
    }
};

#endif
