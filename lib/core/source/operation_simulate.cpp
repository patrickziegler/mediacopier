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

#include <thread>

static constexpr const char* PATTERN_TIMESTAMP = "{:%Y-%m-%d %H:%M:%S}";

namespace mediacopier {

auto FileOperationSimulate::dumpFilePaths(const AbstractFileInfo& file) const -> void
{
    std::ostringstream os;
    auto tp = file.timestamp();
    os << std::vformat(PATTERN_TIMESTAMP, std::make_format_args(tp));
    spdlog::info("{} ({}, Offset: {} min) -> {}",
        file.path().filename().string(), os.str(),
        file.offset().count(),
        m_destination.string());
}

auto FileOperationSimulate::visit(const FileInfoImage& file) -> void
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    dumpFilePaths(file);
}

auto FileOperationSimulate::visit(const FileInfoImageJpeg& file) -> void
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    dumpFilePaths(file);
}

auto FileOperationSimulate::visit(const FileInfoVideo& file) -> void
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    dumpFilePaths(file);
}

} // namespace mediacopier
