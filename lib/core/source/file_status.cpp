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

#include <mediacopier/file_status.hpp>

#if defined(__linux__)
#include <sys/xattr.h>
#endif

#include <array>
#include <string>

namespace fs = std::filesystem;

namespace mediacopier {

namespace {

constexpr const char* STATUS_ATTRIBUTE = "user.mediacopier.status";

} // namespace

auto to_string(FileStatus status) -> std::string_view
{
    switch (status) {
    case FileStatus::Copied:
        return "copied";
    case FileStatus::Duplicate:
        return "duplicate";
    case FileStatus::Error:
        return "error";
    case FileStatus::Unsupported:
        return "unsupported";
    case FileStatus::Unknown:
    default:
        return "unknown";
    }
}

auto from_string(std::string_view value) -> FileStatus
{
    if (value == "copied") {
        return FileStatus::Copied;
    }
    if (value == "duplicate") {
        return FileStatus::Duplicate;
    }
    if (value == "error") {
        return FileStatus::Error;
    }
    if (value == "unsupported") {
        return FileStatus::Unsupported;
    }
    return FileStatus::Unknown;
}

auto get_file_status(const fs::path& path) -> FileStatus
{
#if defined(__linux__)
    std::array<char, 32> buffer {};
    const auto size = ::getxattr(path.c_str(), STATUS_ATTRIBUTE, buffer.data(), buffer.size());
    if (size <= 0) {
        return FileStatus::Unknown;
    }
    return from_string(std::string_view { buffer.data(), static_cast<size_t>(size) });
#else
    (void)path;
    return FileStatus::Unknown;
#endif
}

auto set_file_status(const fs::path& path, FileStatus status) -> bool
{
#if defined(__linux__)
    const auto value = to_string(status);
    return ::setxattr(path.c_str(), STATUS_ATTRIBUTE, value.data(), value.size(), 0) == 0;
#else
    (void)path;
    (void)status;
    return false;
#endif
}

auto clear_file_status(const fs::path& path) -> bool
{
#if defined(__linux__)
    return ::removexattr(path.c_str(), STATUS_ATTRIBUTE) == 0;
#else
    (void)path;
    return false;
#endif
}

} // namespace mediacopier
