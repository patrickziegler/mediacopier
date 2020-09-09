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

#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>
#include <mediacopier/FileOperationPrint.hpp>

#include <iostream>

namespace mc = MediaCopier;

int mc::FileOperationPrint::visit(const mc::FileInfoImage &file) const
{
    std::cout << file.path().string() << std::endl;
    std::cout << "this was an image" << std::endl;
    return 0;
}

int mc::FileOperationPrint::visit(const mc::FileInfoJpeg &file) const
{
    std::cout << file.path().string() << std::endl;
    std::cout << m_filePathFormat.createPathFrom(file).string() << std::endl;
    return 0;
}
int mc::FileOperationPrint::visit(const mc::FileInfoVideo &file) const
{
    std::cout << file.path().string() << std::endl;
    std::cout << "this was a video" << std::endl;
    return 0;
}
