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

#include <mediacopier/error.hpp>
#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/files/image_jpeg.hpp>
#include <mediacopier/files/video.hpp>

#include <spdlog/spdlog.h>

namespace mediacopier {

auto FileInfoFactory::createFromPath(const std::filesystem::path& path) -> FileInfoPtr
{
    try {
        auto image = Exiv2::ImageFactory::open(path);

        if (image->supportsMetadata(Exiv2::MetadataId::mdExif)) {
            image->readMetadata();

            if (image->mimeType() == "image/jpeg") {
                try {
                    return std::make_unique<FileInfoImageJpeg>(path, image->exifData());

                }  catch (const FileInfoImageJpegError& err) {
                    spdlog::warn(std::string{err.what()} + ": " + path.string());
                }
            }

            return std::make_unique<FileInfoImage>(path, image->exifData());
        }

    } catch (const FileInfoError&) {
        return nullptr;

    } catch (const Exiv2::Error&) {
        // this was not an image file
    }

    try {
        return std::make_unique<FileInfoVideo>(path);
    }  catch (const FileInfoError&) {
        // this was not a video file
    }

    return nullptr;
}

} // namespace mediacopier
