/* Copyright (C) 2021 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/operation_simulate.hpp>

#include <mediacopier/file_info_image_jpeg.hpp>
#include <mediacopier/file_info_video.hpp>
#include <spdlog/spdlog.h>

namespace mediacopier {

auto FileOperationSimulate::dumpFilePaths(const AbstractFileInfo& file) const -> void
{
    spdlog::info(m_destination.string() + " (from " + file.path().string() + ")");
}

auto FileOperationSimulate::visit(const FileInfoImage& file) -> void
{
    dumpFilePaths(file);
}

auto FileOperationSimulate::visit(const FileInfoImageJpeg& file) -> void
{
    dumpFilePaths(file);
}

auto FileOperationSimulate::visit(const FileInfoVideo& file) -> void
{
    dumpFilePaths(file);
}

} // namespace mediacopier
