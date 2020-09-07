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

#include <mediacopier/core/FileFactory.hpp>
#include <mediacopier/core/FileImage.hpp>
#include <mediacopier/core/FileVideo.hpp>

namespace mcc = MediaCopier::Core;

std::unique_ptr<mcc::AbstractFile> mcc::FileFactory::createFileFrom(const std::filesystem::path &path) const
{
    try {
        return std::make_unique<FileImage>(path);
    }  catch (const std::runtime_error&) {
        // this was not an image file
    }

    try {
        return std::make_unique<FileVideo>(path);
    }  catch (const std::runtime_error&) {
        // this was not a video file
    }

    throw std::runtime_error("Unknown file type");
}
