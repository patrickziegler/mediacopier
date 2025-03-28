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

#include <mediacopier/operation_move_jpeg.hpp>

#include <mediacopier/file_info_video.hpp>
#include <mediacopier/operation_copy_jpeg.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace mediacopier {

constexpr static const auto upright = FileInfoImageJpeg::Orientation::ROT_0;

auto FileOperationMoveJpeg::moveFileJpeg(const FileInfoImageJpeg& file) const -> void
{
    std::error_code err;
    fs::create_directories(m_destination.parent_path(), err);
    if (err.value()) {
        spdlog::warn("Could not create parent path ({0}): {1}", m_destination.parent_path().string(), err.message());
        return;
    }
    if (file.orientation() != upright && copy_rotate_jpeg(file, m_destination) && reset_exif_orientation(m_destination)) {
        fs::remove(file.path(), err);
        if (err) {
            spdlog::warn("Failed to remove the original file: ({0}): {1}", file.path().string(), err.message());
        }
        return;
    }
    spdlog::warn("Fallback to regular move operation for {}", file.path().string());
    moveFile(file);
}

auto FileOperationMoveJpeg::visit(const FileInfoImage& file) -> void
{
    moveFile(file);
}

auto FileOperationMoveJpeg::visit(const FileInfoImageJpeg& file) -> void
{
    moveFileJpeg(file);
}

auto FileOperationMoveJpeg::visit(const FileInfoVideo& file) -> void
{
    moveFile(file);
}

} // namespace mediacopier
