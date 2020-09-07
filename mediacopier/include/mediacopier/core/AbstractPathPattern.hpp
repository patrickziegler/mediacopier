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

#pragma once

#include <filesystem>
#include <string>
#include <utility>

namespace MediaCopier::Core {

class AbstractFile;

class AbstractPathPattern {
public:
    explicit AbstractPathPattern(std::string pattern)
        : m_pattern(std::move(pattern)) {}
    virtual ~AbstractPathPattern() = default;
    virtual std::filesystem::path createPathFrom(const AbstractFile &file) const = 0;
protected:
    std::string m_pattern;
};

}
