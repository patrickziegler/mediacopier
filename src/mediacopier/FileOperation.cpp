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


#include "FileMeta.hpp"
#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"

namespace bf = boost::filesystem;
namespace bt = boost::posix_time;

using ret = std::tuple<bt::ptime, std::string, int>;

std::shared_ptr<FileOperationStrategy> FileOperation::strategy;
bf::path FileOperation::pathPrefix;
std::locale FileOperation::pathFormat;

int FileOperation::readExifMeta()
{
    try {
        std::tie(timestamp, mimeType, orientation) = read_exif_meta(pathOld);
        return 0;
    } catch (std::invalid_argument&) {
        return 1;
    }
}

int FileOperation::readVideoMeta()
{
    try {
        timestamp = read_video_meta(pathOld);
        return 0;
    } catch (std::invalid_argument&) {
        return 1;
    }
}

FileOperation::FileOperation(const bf::path& file)
{
    pathOld = file;

    if (readExifMeta() && readVideoMeta()) {
        throw std::invalid_argument("No metadata found in " + file.string());
    }

    std::ostringstream buf;
    buf.imbue(pathFormat);
    buf << timestamp;

    pathNew = pathPrefix / bf::path(buf.str() + file.extension().string());
}

FileOperation::FileOperation(const bf::path& file_in, const bf::path& file_out)
{
    pathOld = file_in;

    if (readExifMeta() && readVideoMeta()) {
        throw std::invalid_argument("No metadata found in " + file_in.string());
    }

    pathNew = file_out;
}

void FileOperation::setStrategy(const std::shared_ptr<FileOperationStrategy>& strategy)
{
    FileOperation::strategy = strategy;
}

void FileOperation::setPathPrefix(const bf::path& prefix)
{
    FileOperation::pathPrefix = prefix;
}

void FileOperation::setPathFormat(const std::string& format)
{
    FileOperation::pathFormat = std::locale(std::locale(""), new bt::time_facet(format.c_str()));
}

boost::filesystem::path FileOperation::getPathOld() const
{
    return pathOld;
}

boost::filesystem::path FileOperation::getPathNew() const
{
    return pathNew;
}

std::string FileOperation::getMimeType() const
{
    return mimeType;
}

int FileOperation::getOrientation() const
{
    return orientation;
}

int FileOperation::execute() {
    return strategy->execute(*this);
}
