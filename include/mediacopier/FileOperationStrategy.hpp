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
