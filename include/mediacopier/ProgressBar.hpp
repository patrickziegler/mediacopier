#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <string>
#include <mutex>

class ProgressBar
{
    size_t n;
    size_t i = 0;
    size_t width = 40;
    std::string prefix = "Progress";
    std::mutex mtx;
    void init(size_t);

public:
    ProgressBar(size_t n) : n(n) {}
    ProgressBar(size_t n, std::string prefix) : n(n), prefix(prefix) {}
    ProgressBar(size_t n, std::string prefix, size_t width) : n(n), prefix(prefix) { init(width); }
    ProgressBar(size_t n, size_t width) : n(n) { init(width); }
    void update();
};

#endif
