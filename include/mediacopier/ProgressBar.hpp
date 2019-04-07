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


#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <string>

class ProgressBar
{
    size_t n;
    size_t i = 0;
    size_t width = 50;
    size_t lastPos = 0;
    std::string prefix = "Running";

    void init();

public:
    ProgressBar(size_t n) : n(n) { init(); }
    ProgressBar(size_t n, std::string prefix) : n(n), prefix(prefix) { init(); }
    ProgressBar(size_t n, std::string prefix, size_t width) : n(n), width(width), prefix(prefix) { init(); }
    ProgressBar(size_t n, size_t width) : n(n), width(width) { init(); }

    void update();
    void close();
};

#endif
