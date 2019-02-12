#include "ProgressBar.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>

void ProgressBar::init()
{
    width = (width - prefix.length() - 13);
    std::cout << prefix << " ...\r";
    std::cout.flush();
}

void ProgressBar::update()
{
    ++i;
    double p = static_cast<double>(i) / static_cast<double>(n);
    size_t pos = static_cast<size_t>(p * width);

    if (pos > lastPos) {
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
        lastPos = pos;
    }
}

void ProgressBar::close()
{
    std::cout << std::endl;
}
