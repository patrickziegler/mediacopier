// Copyright (C) 2019 Patrick Ziegler
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#ifndef FILEMETA_H
#define FILEMETA_H

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <tuple>

boost::posix_time::ptime
read_video_meta(const boost::filesystem::path&);

std::tuple<boost::posix_time::ptime, std::string, int>
read_exif_meta(const boost::filesystem::path&);

int reset_exif_orientation(const boost::filesystem::path&);

#endif
