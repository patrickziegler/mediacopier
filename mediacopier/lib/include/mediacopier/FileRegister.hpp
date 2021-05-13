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

#include <mediacopier/FileInfoFactory.hpp>

#include <filesystem>
#include <unordered_map>

namespace MediaCopier {

class AbstractFileInfo;

using FileInfoMap = std::unordered_map<std::string, std::shared_ptr<AbstractFileInfo>>;

class FileRegister {
public:
    explicit FileRegister(std::filesystem::path destination, std::string pattern);
    void add(const std::filesystem::path& path);
    FileInfoMap::const_iterator begin() const;
    FileInfoMap::const_iterator end() const;
    size_t size() const;
private:
    std::filesystem::path m_destdir;
    FileInfoFactory m_factory;
    std::string m_pattern;
    FileInfoMap m_register;
};

}
