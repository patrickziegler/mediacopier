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

#include <mediacopier/operation_copy.hpp>

#include <mediacopier/file_info_image_jpeg.hpp>
#include <mediacopier/file_info_video.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace mediacopier {

auto FileOperationCopy::copyFile(const AbstractFileInfo& file) const -> void
{
    std::error_code err;
    fs::create_directories(m_destination.parent_path(), err);
    if (err.value()) {
        spdlog::warn("Could not create parent path (%s): %s", m_destination.parent_path().string(), err.message());
        return;
    }
    fs::copy_file(file.path(), m_destination, fs::copy_options::overwrite_existing, err);
    if (err.value()) {
        spdlog::warn("Could not copy file (%s): %s", file.path().string(), err.message());
    }
}

auto FileOperationCopy::visit(const FileInfoImage& file) -> void
{
    copyFile(file);
}

auto FileOperationCopy::visit(const FileInfoImageJpeg& file) -> void
{
    copyFile(file);
}

auto FileOperationCopy::visit(const FileInfoVideo& file) -> void
{
    copyFile(file);
}

} // namespace mediacopier
