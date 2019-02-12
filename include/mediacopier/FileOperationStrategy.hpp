#ifndef FILEOPERATIONSTRATEGY_H
#define FILEOPERATIONSTRATEGY_H

#include <vector>
#include <mutex>
#include <boost/filesystem.hpp>

class FileOperation;

struct FileOperationStrategy
{
    const std::string description;
    FileOperationStrategy(std::string description) : description(description) {}
    virtual ~FileOperationStrategy();
    virtual int execute(const FileOperation&) =0;
};

struct FileCopyOverwrite : public FileOperationStrategy
{
    FileCopyOverwrite() : FileOperationStrategy("Copying") {}
    int execute(const FileOperation&);
};

struct FileCopy : public FileOperationStrategy
{
    FileCopy() : FileOperationStrategy("Copying") {}
    int execute(const FileOperation&);
};

struct FileMoveOverwrite : public FileOperationStrategy
{
    FileMoveOverwrite() : FileOperationStrategy("Moving") {}
    int execute(const FileOperation&);
};

struct FileMove : public FileOperationStrategy
{
    FileMove() : FileOperationStrategy("Moving") {}
    int execute(const FileOperation&);
};

struct FileSimulationOverwrite : public FileOperationStrategy
{
    FileSimulationOverwrite() : FileOperationStrategy("Simulating") {}
    int execute(const FileOperation&);
};

class FileSimulation : public FileOperationStrategy
{
    std::mutex mtx;
    std::vector<boost::filesystem::path> filesDone;
public:
    FileSimulation() : FileOperationStrategy("Simulating") {}
    int execute(const FileOperation&);
};

#endif
