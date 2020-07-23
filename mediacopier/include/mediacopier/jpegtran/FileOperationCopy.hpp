#pragma once

#include <mediacopier/core/FileOperationCopy.hpp>

namespace MediaCopier::JpegTran {

class FileImage;

class FileOperationCopy : public Core::FileOperationCopy {
public:
    using Core::FileOperationCopy::FileOperationCopy;
    using Core::FileOperationCopy::accept;
    int accept(const std::shared_ptr<const FileImage> &file) const;
};

}
