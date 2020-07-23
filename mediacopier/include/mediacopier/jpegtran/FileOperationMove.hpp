#pragma once

#include <mediacopier/core/FileOperationMove.hpp>

namespace MediaCopier::JpegTran {

class FileImage;

class FileOperationMove : public Core::FileOperationMove {
public:
    using Core::FileOperationMove::FileOperationMove;
    using Core::FileOperationMove::accept;
    int accept(const std::shared_ptr<const FileImage> &file) const;
};

}
