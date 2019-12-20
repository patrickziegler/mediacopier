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


#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <string>

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

    int readExifMeta();
    int readVideoMeta();

public:
    static void setStrategy(const std::shared_ptr<FileOperationStrategy>&);
    static void setPathPrefix(const boost::filesystem::path&);
    static void setPathFormat(const std::string&);

    FileOperation(const boost::filesystem::path&);
    FileOperation(const boost::filesystem::path&, const boost::filesystem::path&);
    boost::filesystem::path getPathOld() const;
    boost::filesystem::path getPathNew() const;
    std::string getMimeType() const;
    int getOrientation() const;
    int execute();
};

#endif
