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

#pragma once

#include <mediacopier/abstract_file_info.hpp>

#include <filesystem>
#include <optional>
#include <unordered_map>
#include <vector>

namespace mediacopier {

using FileInfoMap = std::unordered_map<std::string, FileInfoPtr>;
using FileConflictMap = std::unordered_map<std::string, std::vector<std::filesystem::path>>;

class FileRegister {
public:
    explicit FileRegister(std::filesystem::path destination, std::string pattern);
    auto add(FileInfoPtr file) -> std::optional<std::filesystem::path>;
    auto removeDuplicates() -> void;
private:
    auto constructDestinationPath(const FileInfoPtr&, size_t) const -> std::filesystem::path;
    std::filesystem::path m_destdir;
    std::string m_pattern;
    FileInfoMap m_register;
    FileConflictMap m_conflicts;
};

} // namespace mediacopier
