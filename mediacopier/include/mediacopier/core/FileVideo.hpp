#pragma once

#include <mediacopier/core/AbstractFile.hpp>

namespace MediaCopier::Core {

class FileVideo : public AbstractFile {
public:
    using AbstractFile::AbstractFile;
    int visit(const AbstractFileOperation& operation) const override;
};

}
