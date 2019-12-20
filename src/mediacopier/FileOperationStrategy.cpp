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

int create_path_directories(const FileOperation& request)
{
    bf::path pathNewDir(request.getPathNew());
    pathNewDir.remove_filename();
    try {
        bf::create_directories(pathNewDir);
        return 0;
    } catch (const bf::filesystem_error&) {
        return 1;
    }
}

int compare_files(bf::path file1, bf::path file2)
{
    const std::streamsize buffer_size = 1024;
    std::vector<char> buffer(buffer_size, '\0');

    std::ifstream input1(file1.string(),  std::ios::in | std::ios::binary);
    std::ifstream input2(file2.string(),  std::ios::in | std::ios::binary);

    std::string chunk1, chunk2;

    while (!(input1.fail() || input2.fail())) {
        input1.read(buffer.data(), buffer_size);
        chunk1 = {buffer.begin(), buffer.begin() + input1.gcount()};

        input2.read(buffer.data(), buffer_size);
        chunk2 = {buffer.begin(), buffer.begin() + input2.gcount()};

        if (chunk1 != chunk2) {
            return 1;
        }
    }
    return 0;
}

int copy_and_fix_jpeg_file(const FileOperation& request, const bf::copy_option& copy_option)
{
    if (jpeg_copy_rotated(request, copy_option) || reset_exif_orientation(request.getPathNew())) {
        return 1;
    } else {
        return 0;
    }
}

int copy_file(const FileOperation& request, const bf::copy_option& copy_option)
{
    if (create_path_directories(request)) {
        return 4;
    }

    if (request.getMimeType() == "image/jpeg" && request.getOrientation() > 2) {

        if (bf::exists(request.getPathNew())) {

            bf::path tmp = bf::unique_path();
            int result;

            FileOperation request_tmp = FileOperation(request.getPathOld(), tmp);

            if (!copy_and_fix_jpeg_file(request_tmp, copy_option)) {
                FileOperation request_new = FileOperation(tmp, request.getPathNew());
                result = copy_file(request_new, copy_option);
                bf::remove(tmp);
                return result;
            }

        } else if (!copy_and_fix_jpeg_file(request, copy_option)) {
            return 0;
        }
    }

    if (bf::exists(request.getPathNew())) {
        if (!compare_files(request.getPathOld(), request.getPathNew())) {
            return -1;
        } else if (copy_option == bf::copy_option::fail_if_exists) {
            return 2;
        }
    }

    try {
        bf::copy_file(request.getPathOld(), request.getPathNew(), copy_option);
        return 0;
    } catch (const bf::filesystem_error&) {
        return 1;
    }
}

int move_file(const FileOperation& request, const bf::copy_option& copy_option)
{
    int result = copy_file(request, copy_option);

    if (result <= 0) {
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

    if (bf::exists(request.getPathNew())) {
        if (compare_files(request.getPathOld(), request.getPathNew())) {
            return 2;
        } else {
            filesDone.push_back(request.getPathNew());
            return -1;
        }
    } else if (std::find(filesDone.begin(), filesDone.end(), request.getPathNew()) != filesDone.end()) {
        return -1;
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
    case -1:
        buf << "IGNORED: Duplicate";
        break;
    case 0:
        buf << "OK";
        break;
    case 1:
        buf << "FAILED: Could not copy / override file";
        break;
    case 2:
        buf << "FAILED: Another file with the same name already exists";
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
