/* Copyright (C) 2022 Patrick Ziegler
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

#include "core/status.hpp"

const Config::Command& Status::command() const {
    return m_command;
}

const std::filesystem::path& Status::inputPath() const {
    return m_inputPath;
}

const std::filesystem::path& Status::outputPath() const {
    return m_outputPath;
}

const size_t& Status::fileCount() const {
    return m_fileCount;
}

const size_t& Status::progress() const {
    return m_progress;
}
