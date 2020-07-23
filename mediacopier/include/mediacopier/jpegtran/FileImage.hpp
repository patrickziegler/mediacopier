#pragma once

#include <mediacopier/core/FileImage.hpp>

namespace MediaCopier::JpegTran {

class FileImage : public Core::FileImage {
public:
    using Core::FileImage::FileImage;
    int visit(const Core::AbstractFileOperation& operation) const override;
};

}
