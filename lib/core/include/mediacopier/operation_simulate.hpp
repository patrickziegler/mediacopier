/* Copyright (C) 2021 Patrick Ziegler <zipat@proton.me>
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
#include <mediacopier/abstract_operation.hpp>

#include <filesystem>

namespace mediacopier {

class FileOperationSimulate : public AbstractFileOperation {
public:
    explicit FileOperationSimulate(std::filesystem::path destination) : m_destination{std::move(destination)} {}
    auto visit(const FileInfoImage& file) -> void override;
    auto visit(const FileInfoImageJpeg& file) -> void override;
    auto visit(const FileInfoVideo& file) -> void override;
protected:
    auto dumpFilePaths(const AbstractFileInfo& file) const -> void;
    std::filesystem::path m_destination;
};

} // namespace mediacopier
