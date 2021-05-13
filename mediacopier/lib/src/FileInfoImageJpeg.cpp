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

#include <mediacopier/AbstractFileOperation.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>

namespace MediaCopier {

FileInfoImageJpeg::FileInfoImageJpeg(std::filesystem::path path, Exiv2::ExifData& exif) : FileInfoImage{std::move(path), exif}
{
    std::string key{"Exif.Image.Orientation"};
    if (exif.findKey(Exiv2::ExifKey{key}) != exif.end()) {
        m_orientation = exif[key].toLong();
    }
}

void FileInfoImageJpeg::accept(const AbstractFileOperation& operation) const
{
    operation.visit(*this);
}

}
