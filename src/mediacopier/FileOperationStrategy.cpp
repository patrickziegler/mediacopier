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


#include "FileJpeg.hpp"
#include "FileMeta.hpp"
#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"

namespace bf = boost::filesystem;

int copy_file(const FileOperation& request, const bf::copy_option& copy_option)
{
    bf::path pathNewDir(request.getPathNew());
    pathNewDir.remove_filename();

    try {
        bf::create_directories(pathNewDir);

    } catch (const bf::filesystem_error&) {
        return 4;
    }

    if (request.getMimeType() != "image/jpeg"
            || request.getOrientation() < 2
            || jpeg_copy_rotated(request, copy_option)
            || reset_exif_orientation(request.getPathNew())) {

        try {
            bf::copy_file(request.getPathOld(), request.getPathNew(), copy_option);

        } catch (const bf::filesystem_error&) {
            return 1;
        }
    }

    return 0;
}

int move_file(const FileOperation& request, const bf::copy_option& copy_option)
{
    int result = copy_file(request, copy_option);

    if (!result) {

        try {
            bf::remove(request.getPathOld());

        } catch (const bf::filesystem_error&) {
            return 3;
        }
    }

    return result;
}

FileOperationStrategy::~FileOperationStrategy() {}

int FileCopyOverwrite::execute(const FileOperation& request)
{
    return copy_file(request, bf::copy_option::overwrite_if_exists);
}

int FileCopy::execute(const FileOperation& request)
{
    return copy_file(request, bf::copy_option::fail_if_exists);
}

int FileMoveOverwrite::execute(const FileOperation& request)
{
    return move_file(request, bf::copy_option::overwrite_if_exists);
}

int FileMove::execute(const FileOperation& request)
{
    return move_file(request, bf::copy_option::fail_if_exists);
}

int FileSimulationOverwrite::execute(const FileOperation&)
{
    return 0;
}

int FileSimulation::execute(const FileOperation& request)
{
    std::lock_guard<std::mutex> lck(mtx);
    bf::path path = request.getPathNew();

    if (bf::exists(path) || (std::find(filesDone.begin(), filesDone.end(), path) != filesDone.end())) {
        return 2;
    } else {
        filesDone.push_back(request.getPathNew());
        return 0;
    }
}

std::string FileOperationStrategy::getLogMessage(int code) const
{
    std::ostringstream buf;

    buf << "File operation [" << description << "] ";

    switch (code)
    {
    case 0:
        buf << "OK";
        break;
    case 1:
        buf << "FAILED: Could not copy / override file";
        break;
    case 2:
        buf << "FAILED: File already exists";
        break;
    case 3:
        buf << "FAILED: File could not be deleted";
        break;
    case 4:
        buf << "FAILED: Could not create directories";
        break;
    default:
        buf << "FAILED: Unknown error number";
        break;
    }

    return buf.str();
}
