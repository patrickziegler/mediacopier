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
#include <mediacopier/FileRegister.hpp>

#include <random>
#include <sstream>

namespace fs = std::filesystem;

namespace MediaCopier {

FileRegister::FileRegister(fs::path destination, std::string pattern) : m_destdir{std::move(destination)}, m_pattern{std::move(pattern)}
{
    m_destdir /= ""; // this will append a trailing directory separator when necessary
}

std::filesystem::path FileRegister::create_path(const AbstractFileInfo &file, unsigned int id, bool useSubsec)
{
    std::stringstream ss;
    ss << m_destdir.string();

    auto ts = std::chrono::system_clock::to_time_t(file.timestamp());
    ss << std::put_time(std::gmtime(&ts), m_pattern.c_str());

    if (useSubsec) {
        // TODO: why is there '% 1000000' in the following line?
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(file.timestamp().time_since_epoch()) % 1000000;
        ss << std::setfill('0') << std::setw(6) << us.count();
    }

    if (id > 0) {
        ss << "_" << id;
    }

    ss << file.path().extension().string();

    return {ss.str()};
}

void FileRegister::add(const std::filesystem::path& path)
{
    auto infoPtr = m_factory.createFromPath(path);
    auto newPath = create_path(*infoPtr, 0, true);

    // TODO: handle conflicts
    m_register[newPath.string()] = std::move(infoPtr);
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
