#pragma once

#include <mediacopier/core/FileFactory.hpp>

#include <filesystem>
#include <memory>

namespace MediaCopier::JpegTran {

class FileFactory : Core::FileFactory {
public:
    std::shared_ptr<Core::AbstractFile> create(const std::filesystem::path &path) const override;
};

}
