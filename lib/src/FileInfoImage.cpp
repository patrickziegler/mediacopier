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

#include <mediacopier/AbstractFileOperation.hpp>
#include <mediacopier/FileOperationPrint.hpp>

#include <date/date.h>
#include <exiv2/exiv2.hpp>

namespace mc = MediaCopier;

mc::FileInfoImage::FileInfoImage(std::filesystem::path path) : AbstractFileInfo(path)
{
    std::unique_ptr<Exiv2::Image> image;

    try {
        image = Exiv2::ImageFactory::open(path.string());
    }  catch (const Exiv2::Error&) {
        throw std::runtime_error("Wrong filetype");
    }

    if (!image->supportsMetadata(Exiv2::MetadataId::mdExif)) {
        throw std::runtime_error("No Exif metadata found");
    }

    try {
        image->readMetadata();
        std::istringstream ss{image->exifData()["Exif.Image.DateTime"].toString()};
        ss >> date::parse("%Y:%m:%d %H:%M:%S", m_timestamp);
    } catch (const Exiv2::Error&) {
        throw std::runtime_error("Error while parsing metadata");
    }
}

int mc::FileInfoImage::accept(const AbstractFileOperation& operation) const
{
    return operation.visit(*this);
}
