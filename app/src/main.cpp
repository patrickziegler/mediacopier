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

#include <iostream>
#include <filesystem>
#include <memory>

#include <mediacopier/core/AbstractFile.hpp>
#include <mediacopier/core/FileFactory.hpp>
#include <mediacopier/core/PathPattern.hpp>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    fs::path root("/home/patrick/Bilder/Wallpaper/");

    std::unique_ptr<MediaCopier::Core::AbstractFile> file;
    MediaCopier::Core::FileFactory fileFactory;

    MediaCopier::Core::PathPattern pathPattern("%Y/%m/%d/IMG_%Y%m%d_%H%M%S");

    for(const auto& path : fs::recursive_directory_iterator(root)) {
        if (path.is_regular_file()) {
            file = fileFactory.createFileFrom(path);
            std::cout << file->path().string() << std::endl;
            std::cout << pathPattern.createPathFrom(*file).string() << std::endl;
        }
    }

    return 0;
}
