#pragma once

#include <filesystem>
#include <string>
#include <utility>

namespace MediaCopier::Core {

class AbstractFile;

class AbstractPathPattern {
    std::string m_pattern;
public:
    explicit AbstractPathPattern(std::string pattern)
        : m_pattern(std::move(pattern)) {}
    virtual ~AbstractPathPattern() = default;
    virtual std::filesystem::path createPathFrom(const AbstractFile &file) const = 0;
};

}
