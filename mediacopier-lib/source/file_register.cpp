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

#include <mediacopier/file_register.hpp>

#include <date/date.h>
#include <mediacopier/duplicate_check.hpp>
#include <mediacopier/error.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace mediacopier {

FileRegister::FileRegister(fs::path destination, std::string pattern, bool useUtc) : m_destdir{std::move(destination)}, m_pattern{std::move(pattern)}, m_useUtc{useUtc}
{
    m_destdir /= ""; // this will append a trailing directory separator when necessary
}

auto FileRegister::add(FileInfoPtr file) -> std::optional<fs::path>
{
    std::vector<std::filesystem::path> conflicts;
    size_t suffix = 0;

    while (suffix < std::numeric_limits<size_t>::max()) {
        auto dest = constructDestinationPath(file, suffix);
        auto item = m_register.find(dest.string());

        if (item != m_register.end()) {
            const auto& knownFile = item->second;
            if (is_duplicate(file->path(), knownFile->path())) {
                spdlog::info("Ignoring duplicate: {0} (same as {1})", file->path().filename().string(), knownFile->path().filename().string());
                return {};
            }
            // possible duplicate of 'item' at destination
            conflicts.push_back(item->first);
            ++suffix;
            continue;
        }

        if (fs::exists(dest)) {
            if (is_duplicate(file->path(), dest)) {
                spdlog::info("Ignoring already exsiting: {0} (same as {1})", file->path().filename().string(), dest.filename().string());
                return {};
            }
            // possible duplicate of 'dest'
            conflicts.push_back(dest);
            ++suffix;
            continue;
        }

        if (conflicts.size() > 0) {
            m_conflicts[dest.string()] = std::move(conflicts);
        }

        m_register[dest.string()] = std::move(file);
        return {std::move(dest)};
    }

    throw FileInfoError{"Unable to find unique filename"};
}

auto FileRegister::removeDuplicates() -> void
{
    for (const auto& [path, conflicts] : m_conflicts)
    {
        for (const auto& conflict : conflicts)
        {
            if (fs::exists(path) && fs::exists(conflict) && is_duplicate(path, conflict)) {
                spdlog::info("Removing duplicate: {0} same as {1}", path, conflict.string());
                fs::remove(path);
                break;
            }
        }
    }
}

auto FileRegister::constructDestinationPath(const FileInfoPtr& file, size_t suffix) const -> fs::path
{
    std::ostringstream os;
    os << m_destdir.string();

    std::chrono::system_clock::time_point tp = file->timestamp();
    if (m_useUtc) {
        tp -= file->offset(); // convert local time to utc
    }

    date::to_stream(os, m_pattern.c_str(), tp);

    if (suffix > 0) {
        os << "_" << suffix;
    }
    os << file->path().extension().string();
    return {os.str()};
}

} // namespace mediacopier
