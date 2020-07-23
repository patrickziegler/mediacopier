#pragma once

#include <mediacopier/core/AbstractPathPattern.hpp>

namespace MediaCopier::Core {

class PathPattern : public AbstractPathPattern {
public:
    using AbstractPathPattern::AbstractPathPattern;
    std::filesystem::path createPathFrom(const AbstractFile &file) const override;
};

}
