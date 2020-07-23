#pragma once

#include <mediacopier/core/FileOperationCopy.hpp>

namespace MediaCopier::Core {

class FileOperationMove : public FileOperationCopy {
public:
    using FileOperationCopy::FileOperationCopy;
    int accept(const std::shared_ptr<const AbstractFile> &file) const override;
};

}
