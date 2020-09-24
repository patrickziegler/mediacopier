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

#pragma once

#include <mediacopier/file_info_image.hpp>

namespace MediaCopier {

class FileInfoImageJpeg : public FileInfoImage {
public:
    enum class Orientation {
        ROT_0 = 1,
        ROT_0_MIRRORED,
        ROT_180,
        ROT_180_MIRRORED,
        ROT_90_MIRRORED,
        ROT_90,
        ROT_270_MIRRORED,
        ROT_270,
    };
    FileInfoImageJpeg(std::filesystem::path path, Exiv2::ExifData exif);
    void accept(const AbstractFileOperation& operation) const override;
    int orientation() const { return m_orientation; }
private:
    int m_orientation;
};

}
