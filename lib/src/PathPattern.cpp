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

#include <mediacopier/PathPattern.hpp>

#include <mediacopier/AbstractFileInfo.hpp>

#include <sstream>

namespace mc = MediaCopier;

std::filesystem::path mc::PathPattern::createPathFrom(const mc::AbstractFileInfo &file) const
{
    auto ts = std::chrono::system_clock::to_time_t(file.timestamp());

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&ts), m_pattern.c_str());
    ss << file.path().extension().string();

    return {ss.str()};
}
