#pragma once

#include <filesystem>
#include <memory>

namespace MediaCopier::Core {

class AbstractFileOperation;

class AbstractFile : public std::enable_shared_from_this<AbstractFile> {
    std::filesystem::path m_path;
public:
    AbstractFile(std::filesystem::path path)
        : m_path(std::move(path)) {}
    virtual ~AbstractFile() = default;
    virtual int visit(const AbstractFileOperation& operation) const = 0;
    std::filesystem::path path() const;
};

}
