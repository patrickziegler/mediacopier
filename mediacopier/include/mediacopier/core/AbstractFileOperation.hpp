#pragma once

#include <mediacopier/core/AbstractPathPattern.hpp>

#include <memory>
#include <utility>

namespace MediaCopier::Core {

class AbstractFile;

class AbstractFileOperation {
    std::unique_ptr<AbstractPathPattern> m_pathPattern;
public:
    explicit AbstractFileOperation(std::unique_ptr<AbstractPathPattern> pathPattern)
        : m_pathPattern(std::move(pathPattern)) {}
    virtual ~AbstractFileOperation() = default;
    virtual int accept(const std::shared_ptr<const AbstractFile> &file) const = 0;
};

}
