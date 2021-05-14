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

#include <mediacopier/Error.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>

#include <log4cplus/log4cplus.h>

namespace MediaCopier {

FileInfoPtr FileInfoFactory::createFromPath(const std::filesystem::path& path)
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("createFromPath"));

    try {
        auto image = Exiv2::ImageFactory::open(path);

        if (image->supportsMetadata(Exiv2::MetadataId::mdExif)) {
            image->readMetadata();

            if (image->mimeType() == "image/jpeg") {
                try {
                    return std::make_unique<FileInfoImageJpeg>(path, image->exifData());

                }  catch (const FileInfoImageJpegError& err) {
                    LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(std::string{err.what()} + ": " + path.string()));
                }
            }

            return std::make_unique<FileInfoImage>(path, image->exifData());
        }
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

} // namespace MediaCopier
