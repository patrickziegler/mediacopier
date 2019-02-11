#include "ProgressBar.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>

void ProgressBar::init(size_t width)
{
    this->width = (width - prefix.length() - 13);
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

        if (p < 1) {
            std::cout << buf.str() << "\r";
        } else {
            std::cout << buf.str() << std::endl;
        }

        std::cout.flush();
        lastPos = pos;
    }
}
