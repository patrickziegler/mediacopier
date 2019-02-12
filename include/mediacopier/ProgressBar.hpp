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
