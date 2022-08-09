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

#include <mediacopier/file_info_factory.hpp>

#include <exiv2/exiv2.hpp>
#include <mediacopier/error.hpp>
#include <mediacopier/file_info_image_jpeg.hpp>
#include <mediacopier/file_info_video.hpp>

#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace mediacopier {

auto to_file_info_ptr(const fs::path& path) -> FileInfoPtr
{
    try {
        auto image = Exiv2::ImageFactory::open(path.string());

        if (image->supportsMetadata(Exiv2::MetadataId::mdExif)) {
            image->readMetadata();

            if (image->mimeType() == "image/jpeg") {
                try {
                    return std::make_shared<FileInfoImageJpeg>(path, image->exifData());

                }  catch (const FileInfoImageJpegError& err) {
                    spdlog::warn(std::string{err.what()} + ": " + path.string());
                }
            }

            return std::make_shared<FileInfoImage>(path, image->exifData());
        }

    } catch (const FileInfoError&) {
        return {};

    } catch (const Exiv2::Error&) {
        // this was not an image file
    }

    try {
        return std::make_shared<FileInfoVideo>(path);
    }  catch (const FileInfoError&) {
        // this was not a video file
    }

    spdlog::info("Couldn't find metadata: " + path.string());

    return {};
}

} // namespace mediacopier
