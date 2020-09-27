/* Copyright (C) 2020 Patrick Ziegler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>
#include <mediacopier/FileOperationCopy.hpp>

#include <fstream>

namespace fs = std::filesystem;
namespace mc = MediaCopier;

static bool check_equal(fs::path file1, fs::path file2)
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
            return false;
        }
    }
    return true;
}

void mc::FileOperationCopy::copyFile(const mc::AbstractFileInfo &file) const
{
    std::error_code err;
    unsigned int id = 0;

    while (true) {
        auto dst = m_filePathFormat.createPathFrom(file, id);
        if (!fs::exists(dst)) {
            fs::create_directories(dst.parent_path());
            fs::copy_file(file.path(), dst, err);
            if (err.value() > 0) {
                throw FileOperationError{err.message()};
            }
        }
        if (check_equal(file.path(), dst)) {
            return;
        }
        if (id == std::numeric_limits<unsigned int>::max()) {
            throw FileOperationError{"Unable to find unique filename"};
        }
        ++id;
    }
}

void mc::FileOperationCopy::visit(const mc::FileInfoImage &file) const
{
    copyFile(file);
}

void mc::FileOperationCopy::visit(const mc::FileInfoImageJpeg &file) const
{
    copyFile(file);
}

void mc::FileOperationCopy::visit(const mc::FileInfoVideo &file) const
{
    copyFile(file);
}
