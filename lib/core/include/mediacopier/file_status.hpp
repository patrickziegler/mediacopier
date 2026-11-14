/* Copyright (C) 2026 Patrick Ziegler <zipat@proton.me>
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

#include <filesystem>
#include <string_view>

namespace mediacopier {

/**
 * Processing result that is stored as an extended attribute on the source file.
 * KDE file managers can use this information to display an overlay icon.
 */
enum class FileStatus {
    Unknown,
    Copied,
    Duplicate,
    Error,
    Unsupported,
};

auto to_string(FileStatus status) -> std::string_view;
auto from_string(std::string_view value) -> FileStatus;

auto get_file_status(const std::filesystem::path& path) -> FileStatus;
auto set_file_status(const std::filesystem::path& path, FileStatus status) -> bool;
auto clear_file_status(const std::filesystem::path& path) -> bool;

} // namespace mediacopier
