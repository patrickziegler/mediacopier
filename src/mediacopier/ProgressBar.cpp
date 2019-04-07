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


#include "ProgressBar.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

void ProgressBar::init()
{
    width = (width - prefix.length() - 13);
    std::cout << prefix << " ...\r";
    std::cout.flush();
}

void ProgressBar::showProgress()
{
    double p = static_cast<double>(i) / static_cast<double>(n);
    size_t pos = static_cast<size_t>(p * width);

    std::ostringstream buf;

    buf << prefix
        << " ["
        << std::string(pos, '=')
        << ">"
        << std::string(width - pos, ' ')
        << "] "
        << std::fixed
        << std::setprecision(2)
        << std::setfill(' ')
        << std::setw(6)
        << 100 * p << " %";

    std::cout << buf.str() << "\r";
    std::cout.flush();
}

bool ProgressBar::hasFinished()
{
    return i == n;
}

void ProgressBar::close()
{
    i = n;
    showProgress();
    std::cout << std::endl;
}
