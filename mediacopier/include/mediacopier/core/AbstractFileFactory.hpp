#pragma once

#include <filesystem>
#include <memory>

namespace MediaCopier::Core {

class AbstractFile;

class AbstractFileFactory {
public:
    virtual std::shared_ptr<AbstractFile> create(const std::filesystem::path &path) const = 0;
};

}
