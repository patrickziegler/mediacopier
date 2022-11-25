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
#include <mediacopier/error.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

constexpr static const size_t BUFFER_SIZE = 1024;
constexpr static const size_t CHUNKS_MAX = 32;

static auto seek_jpeg_data(std::ifstream* input) noexcept -> bool
{
    uint8_t s1 = 0, s2 = 0;
    uint16_t buf = 0;

    input->seekg(2, std::ios_base::beg);

    while (input->good()) {
        input->read(reinterpret_cast<char*>(&buf), 2);
        if (buf == 0xdaff) {
            return true;
        }
        input->read(reinterpret_cast<char*>(&s1), 1);
        input->read(reinterpret_cast<char*>(&s2), 1);
        input->seekg(256 * s1 + s2 - 2, std::ios_base::cur);
    }

    return false;
}

static auto prepare_input_stream(const fs::path& file) -> std::unique_ptr<std::ifstream>
{
    if (!fs::exists(file)) {
        throw std::runtime_error(file.string() + " does not exist");
    }
    auto input = std::make_unique<std::ifstream>(file,  std::ios_base::in | std::ios_base::binary);

    uint16_t buf;
    input->read(reinterpret_cast<char*>(&buf), 2);

    if (buf == 0xd8ff) { // is jpeg file
        seek_jpeg_data(input.get());
    }
    return input;
}

static auto is_duplicate(const fs::path& file1, const fs::path& file2) -> bool
{
    const auto input1 = prepare_input_stream(file1);
    const auto input2 = prepare_input_stream(file2);

    std::vector<char> buffer(BUFFER_SIZE, '\0');

    std::string chunk1, chunk2;
    size_t chunks_left = CHUNKS_MAX;

    while (chunks_left > 0) {
        if (!input1->good() && !input2->good()) {
            return false;
        }
        input1->read(buffer.data(), BUFFER_SIZE);
        chunk1 = {buffer.begin(), buffer.begin() + input1->gcount()};

        input2->read(buffer.data(), BUFFER_SIZE);
        chunk2 = {buffer.begin(), buffer.begin() + input2->gcount()};

        if (chunk1 != chunk2) {
            return false;
        }
        --chunks_left;
    }
    return true;
}

namespace mediacopier {

FileRegister::FileRegister(fs::path destination, std::string pattern) : m_destdir{std::move(destination)}, m_pattern{std::move(pattern)}
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
                spdlog::info("Ignoring duplicate: " + file->path().filename().string() + " same as " + knownFile->path().filename().string());
                return {};
            }
            // possible duplicate of 'item' at destination
            conflicts.push_back(item->first);
            ++suffix;
            continue;
        }

        if (fs::exists(dest)) {
            if (is_duplicate(file->path(), dest)) {
                spdlog::info("Ignoring obsolete: " + file->path().filename().string() + " same as " + dest.filename().string());
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
    for (const auto& [path, conflicts] : m_conflicts) {
        for (const auto& conflict : conflicts) {
            if (fs::exists(path) && fs::exists(conflict) && is_duplicate(path, conflict)) {
                spdlog::info("Removing duplicate: " + path + " same as " + conflict.string());
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

    date::to_stream(os, m_pattern.c_str(), file->timestamp());

    if (suffix > 0) {
        os << "_" << suffix;
    }

    os << file->path().extension().string();
    return {os.str()};
}

} // namespace mediacopier
