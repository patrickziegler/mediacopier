/* Copyright (C) 2020-2022 Patrick Ziegler
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

namespace mediacopier {

class AbstractFileOperation;

class AbstractFileInfo {
public:
    AbstractFileInfo(std::filesystem::path path)
        : m_path{std::move(path)} {}
    virtual ~AbstractFileInfo() = default;
    virtual auto accept(AbstractFileOperation& operation) const -> void = 0;
    auto path() const -> std::filesystem::path { return m_path; }
    auto timestamp() const -> std::chrono::system_clock::time_point { return m_timestamp; }
protected:
    std::filesystem::path m_path;
    std::chrono::system_clock::time_point m_timestamp;
};

using FileInfoPtr = std::shared_ptr<AbstractFileInfo>;

} // namespace mediacopier
