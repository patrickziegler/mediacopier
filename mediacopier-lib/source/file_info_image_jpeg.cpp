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

#include <mediacopier/file_info_image_jpeg.hpp>

#include <mediacopier/abstract_operation.hpp>
#include <mediacopier/error.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace mediacopier {

auto reset_exif_orientation(const fs::path& path) noexcept -> bool
{
    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(path.string());
        image->readMetadata();

        auto exif = image->exifData();
        exif["Exif.Image.Orientation"] = static_cast<int>(FileInfoImageJpeg::Orientation::ROT_0);

        image->setExifData(exif);
        image->writeMetadata();

    } catch(const std::exception& err) {
        spdlog::warn("Could not reset exif orientation tag ({0}): {1}", path.string(), err.what());
        return false;
    }

    return true;
}

FileInfoImageJpeg::FileInfoImageJpeg(std::filesystem::path path, Exiv2::ExifData& exif) : FileInfoImage{std::move(path), exif}
{
    const auto& item = exif.findKey(Exiv2::ExifKey{"Exif.Image.Orientation"});

    if (item == exif.end()) {
        throw FileInfoImageJpegError{"Field 'Exif.Image.Orientation' not found in metadata"};
    }

#ifdef EXIV2_HAS_TOLONG
    auto orientation = item->toLong();
#else
    auto orientation = item->toInt64();
#endif

    if (orientation < static_cast<long>(Orientation::ROT_0) || orientation > static_cast<long>(Orientation::ROT_90)) {
        throw FileInfoImageJpegError{"Invalid orientation value"};
    }

    m_orientation = static_cast<Orientation>(orientation);
}

auto FileInfoImageJpeg::accept(AbstractFileOperation& operation) const -> void
{
    operation.visit(*this);
}

} // namespace mediacopier
