/* Copyright (C) 2021 Patrick Ziegler
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

#include <mediacopier/files/image.hpp>
#include <mediacopier/files/image_jpeg.hpp>
#include <mediacopier/files/video.hpp>
#include <mediacopier/operations/abstract_file_operation.hpp>

namespace mediacopier::test {

enum class FileInfoType {
    None,
    FileInfoImage,
    FileInfoImageJpeg,
    FileInfoVideo,
};

class FileInfoTypeDetector : public AbstractFileOperation {
public:
    auto visit(const FileInfoImage& /*file*/) -> void
    {
        m_lastType = FileInfoType::FileInfoImage;
    }

    auto visit(const FileInfoImageJpeg& /*file*/) -> void
    {
        m_lastType = FileInfoType::FileInfoImageJpeg;
    }

    auto visit(const FileInfoVideo& /*file*/) -> void
    {
        m_lastType = FileInfoType::FileInfoVideo;
    }

    auto lastType() const -> FileInfoType
    {
        return m_lastType;
    }

private:
    FileInfoType m_lastType = FileInfoType::None;
};

} // namespace mediacopier
