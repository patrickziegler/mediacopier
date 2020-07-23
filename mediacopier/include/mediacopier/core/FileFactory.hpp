#pragma once

#include <mediacopier/core/AbstractFileFactory.hpp>

#include <filesystem>
#include <memory>

namespace MediaCopier::Core {

class FileFactory : AbstractFileFactory {
public:
    std::shared_ptr<AbstractFile> create(const std::filesystem::path &path) const override;
};

}
