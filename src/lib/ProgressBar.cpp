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
    std::lock_guard<std::mutex> lck(mtx);

    ++i;
    double p = double(i) / double(n);
    size_t pos = size_t(p * width);

    std::ostringstream ss;

    ss << prefix
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
        std::cout << ss.str() << "\r";
    } else {
        std::cout << ss.str() << std::endl;
    }

    std::cout.flush();
}
