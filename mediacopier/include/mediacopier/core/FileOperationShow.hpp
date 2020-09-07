#pragma once

#include <mediacopier/core/AbstractFileOperation.hpp>

namespace MediaCopier::Core {

class FileOperationShow : public AbstractFileOperation {
public:
    using AbstractFileOperation::AbstractFileOperation;
    int accept(const std::shared_ptr<const AbstractFile> &file) const override;
};

}
