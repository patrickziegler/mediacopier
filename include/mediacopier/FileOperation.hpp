#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <string>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

class FileOperationStrategy;

class FileOperation
{
    static std::shared_ptr<FileOperationStrategy> strategy;
    static boost::filesystem::path pathPrefix;
    static std::locale pathFormat;

    boost::filesystem::path pathOld;
    boost::filesystem::path pathNew;
    boost::posix_time::ptime timestamp;
    std::string mimeType = "";
    int orientation = 0;

    int readExif();
    int readVideoMeta();
    int setTimestamp(const std::string&);

public:
    static void setStrategy(const std::shared_ptr<FileOperationStrategy>&);
    static void setPathPrefix(const boost::filesystem::path&);
    static void setPathFormat(const std::string&);

    FileOperation(const boost::filesystem::path&);

    boost::filesystem::path getPathOld() const;
    boost::filesystem::path getPathNew() const;
    std::string getMimeType() const;
    int getOrientation() const;

    std::string getLogMessage(int) const;
    int execute();
};

#endif
