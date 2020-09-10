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
#include <mediacopier/FilePathFormat.hpp>

#include <sstream>

namespace mc = MediaCopier;

mc::FilePathFormat::FilePathFormat(std::filesystem::path destination) : m_destination{std::move(destination)}
{
    m_destination /= ""; // this will append a trailing directory separator when necessary
}

std::filesystem::path mc::FilePathFormat::createPathFrom(const mc::AbstractFileInfo &file) const
{
    // TODO: make these fields configurable
    std::string pattern{"IMG_%Y%m%d_%H%M%S_"};
    bool subsec = true;

    std::stringstream ss;
    ss << m_destination.string();

    auto ts = std::chrono::system_clock::to_time_t(file.timestamp());
    ss << std::put_time(std::gmtime(&ts), pattern.c_str());

    if (subsec) {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(file.timestamp().time_since_epoch()) % 1000000;
        ss << std::setfill('0') << std::setw(6) << us.count();
    }

    ss << file.path().extension().string();

    return {ss.str()};
}
