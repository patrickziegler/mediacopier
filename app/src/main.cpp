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

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/Exceptions.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>
#include <mediacopier/FilePathFormat.hpp>

#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;
namespace mc = MediaCopier;

int main(int argc, char *argv[])
{
    fs::path src{"/home/patrick/workspace/repos/tmp/"};
    fs::path dst{"/home/patrick/workspace/repos/tmp_out/"};

    mc::FileOperationCopyJpeg op{mc::FilePathFormat{dst}};
    mc::FileInfoFactory factory;

    for (const auto& path : fs::recursive_directory_iterator(src)) {
        try {
            if (path.is_regular_file()) {
                auto file = factory.createFileFrom(path);
                file->accept(op);
            }
        }  catch (const mc::FileInfoError& err) {
            std::cout << err.what() << " (" << path << ")" << std::endl;
        }
    }

    return 0;
}
