/* Copyright (C) 2020 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/file_info_image.hpp>

#include <mediacopier/abstract_operation.hpp>
#include <mediacopier/error.hpp>

#include <array>
#include <chrono>

/* 'original' refers to the moment the picture was taken (shutter pressed),
 * 'digitized' is the moment where the picture was scanned (should be the same for digital cams),
 * the plain 'DateTime' is the fallback if the above keys are missing */

static constexpr const std::array<char[29], 3> keysDateTime = {
    "Exif.Photo.DateTimeOriginal",
    "Exif.Photo.DateTimeDigitized",
    "Exif.Image.DateTime",
};

static constexpr const std::array<char[31], 3> keysSubSec = {
    "Exif.Photo.SubSecTimeOriginal",
    "Exif.Photo.SubSecTimeDigitized",
    "Exif.Photo.SubSecTime"
};

static constexpr const std::array<char[31], 3> keysOffset = {
    "Exif.Photo.OffsetTimeOriginal",
    "Exif.Photo.OffsetTimeDigitized",
    "Exif.Photo.OffsetTime",
};

namespace mediacopier {

FileInfoImage::FileInfoImage(std::filesystem::path path, Exiv2::ExifData& exif) : AbstractFileInfo{std::move(path)}
{
    std::string key, value;
    int hours, minutes;
    char colon; // used for parsing timezone offset without scanning for separator

    size_t i = keysDateTime.size() + 1;
    for (size_t j=0; j < keysDateTime.size(); ++j) {
        key = keysDateTime[j];
        if (exif.findKey(Exiv2::ExifKey{key}) != exif.end()) {
            i = j;
            break;
        }
    }
    if (i > keysDateTime.size()) {
        throw FileInfoError{"No date information found"};
    }

    std::stringstream timestamp;
    value = exif[key].toString();
    timestamp << value;

    key = keysSubSec[i];
    if (exif.findKey(Exiv2::ExifKey{key}) != exif.end()) {
        value = exif[key].toString();
        if (value.size() > 0) {
            timestamp << "." << value;
        }
    }
    if (timestamp.str().size() < 1) {
        throw FileInfoError{"No date information found"};
    }

    timestamp >> std::chrono::parse("%Y:%m:%d %T", m_timestamp);
    if (timestamp.fail()) {
        throw FileInfoError{"Invalid date info found"};
    }

    key = keysOffset[i];
    if (exif.findKey(Exiv2::ExifKey{key}) != exif.end()) {
        value = exif[key].toString();
        timestamp.str(value);
        timestamp.clear();
        timestamp >> hours >> colon >> minutes;
        if (hours < 0) {
            minutes *= -1;
        }
        m_offset = std::chrono::hours(hours) + std::chrono::minutes(minutes);
    }
}

auto FileInfoImage::accept(AbstractFileOperation& operation) const -> void
{
    operation.visit(*this);
}

} // namespace mediacopier
