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

#include <stdexcept>

namespace MediaCopier {

class MediaCopierError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FileInfoError : public MediaCopierError {
    using MediaCopierError::MediaCopierError;
};

class FileInfoImageJpegError : public MediaCopierError {
    using MediaCopierError::MediaCopierError;
};

class FileOperationError : public MediaCopierError {
    using MediaCopierError::MediaCopierError;
};

} // namespace MediaCopier
