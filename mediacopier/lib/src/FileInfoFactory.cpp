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

#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>

namespace mc = MediaCopier;

std::unique_ptr<mc::AbstractFileInfo> mc::FileInfoFactory::createFromPath(const std::filesystem::path &path)
{
    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(path);

        if (image->supportsMetadata(Exiv2::MetadataId::mdExif)) {
            image->readMetadata();

            if (image->mimeType() == "image/jpeg") {
                return std::make_unique<FileInfoImageJpeg>(path, std::move(image->exifData()));
            } else {
                return std::make_unique<FileInfoImage>(path, std::move(image->exifData()));
            }
        }
    }  catch (const Exiv2::Error&) {
        // this was not an image file
    }

    try {
        return std::make_unique<FileInfoVideo>(path);
    }  catch (const mc::FileInfoError&) {
        // this was not a video file
    }

    throw mc::FileInfoError{"Unknown file type"};
}
