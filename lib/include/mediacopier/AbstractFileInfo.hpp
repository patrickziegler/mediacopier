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

#include <chrono>
#include <filesystem>

namespace MediaCopier {

class AbstractFileOperation;

class AbstractFileInfo {
public:
    AbstractFileInfo(std::filesystem::path path)
        : m_path(std::move(path)) {}
    virtual ~AbstractFileInfo() = default;
    virtual int accept(const AbstractFileOperation& operation) const = 0;
    std::filesystem::path path() const { return m_path; }
    std::chrono::system_clock::time_point timestamp() const { return m_timestamp; }
protected:
    std::filesystem::path m_path;
    std::chrono::system_clock::time_point m_timestamp;
};

}
