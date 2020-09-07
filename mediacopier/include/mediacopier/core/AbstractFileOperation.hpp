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

#include <mediacopier/core/AbstractPathPattern.hpp>

#include <memory>
#include <utility>

namespace MediaCopier::Core {

class AbstractFile;

class AbstractFileOperation {
    std::unique_ptr<AbstractPathPattern> m_pathPattern;
public:
    explicit AbstractFileOperation(std::unique_ptr<AbstractPathPattern> pathPattern)
        : m_pathPattern(std::move(pathPattern)) {}
    virtual ~AbstractFileOperation() = default;
    virtual int accept(const std::shared_ptr<const AbstractFile> &file) const = 0;
};

}
