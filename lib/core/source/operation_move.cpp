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

#include <mediacopier/operation_move.hpp>

#include <mediacopier/error.hpp>
#include <mediacopier/file_info_image_jpeg.hpp>
#include <mediacopier/file_info_video.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace mediacopier {

auto FileOperationMove::moveFile(const AbstractFileInfo& file) const -> void
{
    std::error_code err;
    fs::create_directories(m_destination.parent_path()); // may throw
    fs::rename(file.path(), m_destination, err);
    if (err.value() == EXDEV) {
        spdlog::debug("Move accross filesystems, fallback to copy + remove approach");
        fs::copy_file(file.path(), m_destination, fs::copy_options::overwrite_existing); // may throw
        fs::remove(file.path(), err);
        if (err) {
            spdlog::warn("Failed to remove the original file: ({0}): {1}", file.path().string(), err.message());
        }
    } else if (err) {
        throw mediacopier::FileOperationError{"Failed to move file " + file.path().string() + ": " + err.message()};
    }
}

auto FileOperationMove::visit(const FileInfoImage& file) -> void
{
    moveFile(file);
}

auto FileOperationMove::visit(const FileInfoImageJpeg& file) -> void
{
    moveFile(file);
}

auto FileOperationMove::visit(const FileInfoVideo& file) -> void
{
    moveFile(file);
}

} // namespace mediacopier
