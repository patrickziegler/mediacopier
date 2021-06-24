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

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/Error.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileRegister.hpp>

#include <log4cplus/loggingmacros.h>

#include <random>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

static bool is_duplicate(fs::path file1, fs::path file2)
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

namespace MediaCopier {

FileRegister::FileRegister(fs::path destination, std::string pattern) : m_destdir{std::move(destination)}, m_pattern{std::move(pattern)}
{
    m_destdir /= ""; // this will append a trailing directory separator when necessary
}

void FileRegister::add(const std::filesystem::path& path)
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("add"));

    size_t id = 0;

    auto file = FileInfoFactory::createFromPath(path);

    if (file == nullptr) {
        throw FileInfoError{"Failed to parse metadata"};
    }

    while (id < std::numeric_limits<size_t>::max()) {

        auto destination = getDestinationPath(*file, id, true);
        auto item = m_register.find(destination);

        if (item != m_register.end()) {
            const auto& knownFile = item->second;
            if (is_duplicate(path, knownFile->path())) {
                LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Duplicate: " + path.filename().string() + " same as " + knownFile->path().filename().string()));
                return;
            }
            ++id;
            continue;
        }

        if (fs::exists(destination)) {
            if (is_duplicate(path, destination)) {
                LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Already there: " + path.filename().string() + " same as " + destination.filename().string()));
                return;
            }
            ++id;
            continue;
        }

        m_register[destination.string()] = std::move(file);
        return;
    }

    throw FileInfoError{"Unable to find unique filename"};
}

void FileRegister::reset()
{
    m_register.clear();
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

std::filesystem::path FileRegister::getDestinationPath(const MediaCopier::AbstractFileInfo& file, size_t id, bool useSubsec) const
{
    std::stringstream ss;
    ss << m_destdir.string();

    auto ts = std::chrono::system_clock::to_time_t(file.timestamp());
    ss << std::put_time(std::gmtime(&ts), m_pattern.c_str());

    if (useSubsec) {
        // '% 1000000' because we search for us "on top" of the number of seconds
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(file.timestamp().time_since_epoch()) % 1000000;
        ss << std::setfill('0') << std::setw(6) << us.count();
    }

    if (id > 0) {
        ss << "_" << id;
    }

    ss << file.path().extension().string();

    return {ss.str()};
}

} // namespace MediaCopier
