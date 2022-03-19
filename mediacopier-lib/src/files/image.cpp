/* Copyright (C) 2020-2021 Patrick Ziegler
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

#include <mediacopier/abstract_file_operation.hpp>
#include <mediacopier/error.hpp>
#include <mediacopier/files/image.hpp>

#include <date/date.h>

#include <array>

static constexpr const std::array<char[29], 4> keysDateTime = {
    "Exif.Photo.DateTimeOriginal",
    "Exif.Image.DateTimeOriginal",
    "Exif.Photo.DateTimeDigitized",
    "Exif.Image.DateTime",
};

static constexpr const std::array<char[31], 3> keysSubSec = {
    "Exif.Photo.SubSecTimeOriginal",
    "Exif.Photo.SubSecTimeDigitized",
    "Exif.Photo.SubSecTime"
};

namespace mediacopier {

FileInfoImage::FileInfoImage(std::filesystem::path path, Exiv2::ExifData& exif) : AbstractFileInfo{std::move(path)}
{
    std::stringstream timestamp;

    for (const std::string& key : keysDateTime) {
        if (exif.findKey(Exiv2::ExifKey{key}) == exif.end()) {
            continue;
        }
        std::string value = exif[key].toString();
        if (value.length() > 0) {
            timestamp << value;
            break;
        }
    }

    if (timestamp.str().size() < 1) {
        throw FileInfoError{"No date information found"};
    }

    for (const std::string& key : keysSubSec) {
        if (exif.findKey(Exiv2::ExifKey{key}) == exif.end()) {
            continue;
        }
        std::string value = exif[key].toString();
        if (value.length() > 0) {
            timestamp << "." << value;
            break;
        }
    }

    timestamp >> date::parse("%Y:%m:%d %H:%M:%S", m_timestamp);
}

auto FileInfoImage::accept(AbstractFileOperation& operation) const -> void
{
    operation.visit(*this);
}

} // namespace mediacopier
