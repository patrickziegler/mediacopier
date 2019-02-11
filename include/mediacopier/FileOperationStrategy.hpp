#ifndef FILEOPERATIONSTRATEGY_H
#define FILEOPERATIONSTRATEGY_H

#include <vector>
#include <mutex>
#include <boost/filesystem.hpp>

class FileOperation;

struct FileOperationStrategy
{
    std::string name = "Nop";
    virtual ~FileOperationStrategy();
    virtual int execute(const FileOperation&) =0;
};

struct FileCopyOverwrite : public FileOperationStrategy
{
    FileCopyOverwrite() { name = "Copying"; }
    int execute(const FileOperation&);
};

class FileCopy : public FileCopyOverwrite
{
    int execute(const FileOperation&);
};

struct FileMoveOverwrite : public FileCopyOverwrite
{
    FileMoveOverwrite() { name = "Moving"; }
    int execute(const FileOperation&);
};

struct FileMove : public FileMoveOverwrite
{
    int execute(const FileOperation&);
};

struct FileSimulationOverwrite : public FileOperationStrategy
{
    FileSimulationOverwrite() { name = "Simulating"; }
    int execute(const FileOperation&);
};

class FileSimulation : public FileOperationStrategy
{
    std::mutex mtx;
    std::vector<boost::filesystem::path> filesDone;
public:
    FileSimulation() { name = "Simulating"; }
    int execute(const FileOperation&);
};

#endif
