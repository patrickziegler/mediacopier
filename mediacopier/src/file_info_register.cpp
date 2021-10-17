/* Copyright (C) 2020-2021 Patrick Ziegler
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

#include <mediacopier/error.hpp>
#include <mediacopier/file_info_register.hpp>

#include <date/date.h>
#include <spdlog/spdlog.h>

#include <random>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

static auto is_duplicate(const fs::path& file1, const fs::path& file2) -> bool
{
    const std::streamsize buffer_size = 1024;
    std::vector<char> buffer(buffer_size, '\0');

    std::ifstream input1(file1.string(),  std::ios::in | std::ios::binary);
    std::ifstream input2(file2.string(),  std::ios::in | std::ios::binary);

    std::string chunk1, chunk2;

    while (!(input1.fail() || input2.fail())) {
        input1.read(buffer.data(), buffer_size);
        chunk1 = {buffer.begin(), buffer.begin() + input1.gcount()};

        input2.read(buffer.data(), buffer_size);
        chunk2 = {buffer.begin(), buffer.begin() + input2.gcount()};

        if (chunk1 != chunk2) {
            return false;
        }
    }

    return true;
}

namespace mediacopier {

FileRegister::FileRegister(fs::path destination, std::string pattern) : m_destdir{std::move(destination)}, m_pattern{std::move(pattern)}
{
    m_destdir /= ""; // this will append a trailing directory separator when necessary
}

auto FileRegister::add(FileInfoPtr file) -> std::optional<std::filesystem::path>
{
    size_t id = 0;

    while (id < std::numeric_limits<size_t>::max()) {
        auto dest = constructDestinationPath(file, id);
        auto item = m_register.find(dest);

        if (item != m_register.end()) {
            const auto& knownFile = item->second;
            if (is_duplicate(file->path(), knownFile->path())) {
                spdlog::warn("Duplicate: " + file->path().filename().string() + " same as " + knownFile->path().filename().string());
                return {};
            }
            ++id;
            continue;
        }

        if (fs::exists(dest)) {
            if (is_duplicate(file->path(), dest)) {
                spdlog::warn("Already there: " + file->path().filename().string() + " same as " + dest.filename().string());
                return {};
            }
            ++id;
            continue;
        }

        m_register[dest.string()] = std::move(file);
        return {std::move(dest)};
    }

    throw FileInfoError{"Unable to find unique filename"};
}

auto FileRegister::constructDestinationPath(const FileInfoPtr& file, size_t id) const -> std::filesystem::path
{
    std::ostringstream os;
    os << m_destdir.string();

    date::to_stream(os, m_pattern.c_str(), file->timestamp());

    if (id > 0) {
        os << "_" << id;
    }

    os << file->path().extension().string();
    return {os.str()};
}

} // namespace mediacopier
