/* Copyright (C) 2020 Patrick Ziegler
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

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileRegister.hpp>

#include <random>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

static bool is_equal(fs::path file1, fs::path file2)
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

static std::filesystem::path create_path(const MediaCopier::AbstractFileInfo& file, const fs::path& m_destdir, const std::string& m_pattern, size_t id, bool useSubsec)
{
    std::stringstream ss;
    ss << m_destdir.string();

    auto ts = std::chrono::system_clock::to_time_t(file.timestamp());
    ss << std::put_time(std::gmtime(&ts), m_pattern.c_str());

    if (useSubsec) {
        // TODO: why '% 1000000' in the following line?
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(file.timestamp().time_since_epoch()) % 1000000;
        ss << std::setfill('0') << std::setw(6) << us.count();
    }

    if (id > 0) {
        ss << "_" << id;
    }

    ss << file.path().extension().string();

    return {ss.str()};
}

namespace MediaCopier {

FileRegister::FileRegister(fs::path destination, std::string pattern) : m_destdir{std::move(destination)}, m_pattern{std::move(pattern)}
{
    m_destdir /= ""; // this will append a trailing directory separator when necessary
}

void FileRegister::add(const std::filesystem::path& path)
{
    size_t id = 0;

    auto infoPtr = m_factory.createFromPath(path);
    fs::path newPath;

    while (id < std::numeric_limits<size_t>::max()) {

        newPath = create_path(*infoPtr, m_destdir, m_pattern, id, true);

        auto item = m_register.find(newPath);

        if (item != m_register.end()) {
            if (is_equal(path, item->second->path())) {
                return; // is duplicate
            }
            ++id;
            continue;
        }

        if (fs::exists(newPath)) {
            if (is_equal(path, newPath)) {
                return; // is duplicate
            }
            ++id;
            continue;
        }

        m_register[newPath.string()] = std::move(infoPtr);
        break;
    }

    throw FileOperationError{"Unable to find unique filename"};
}

FileInfoMap::const_iterator FileRegister::begin() const
{
    return m_register.begin();
}

FileInfoMap::const_iterator FileRegister::end() const
{
    return m_register.end();
}

size_t FileRegister::size() const
{
    return m_register.size();
}

}
