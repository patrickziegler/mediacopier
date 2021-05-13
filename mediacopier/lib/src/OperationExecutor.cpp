/* Copyright (C) 2021 Patrick Ziegler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <mediacopier/OperationExecutor.hpp>

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>

namespace fs = std::filesystem;

namespace MediaCopier {

template <typename T>
void execute(const FileRegister& fileRegister)
{
    for (const auto& item : fileRegister) {
        T operation{item.first};
        item.second->accept(operation);
    }
}

void OperationExecutor::run()
{
    if (!fs::is_directory(m_inputDir)) {
        throw std::runtime_error("Input folder does not exist");
    }

    FileRegister fileRegister{m_outputDir, m_baseFormat};

    for (const auto& path : fs::recursive_directory_iterator(m_inputDir)) {
        if (path.is_regular_file()) {
            try {
                fileRegister.add(path);
            } catch (const FileInfoError& err) {
                // TODO: log this incident (err.what())
            }
        }
    }

    if (fileRegister.size() < 1) {
        throw std::runtime_error("No files were found in " + m_inputDir.string());
    }

    switch (m_command)
    {
    case Command::COPY:
        execute<FileOperationCopyJpeg>(fileRegister);

    case Command::MOVE:
        execute<FileOperationMoveJpeg>(fileRegister);

    default:
        throw std::runtime_error("Unknown operation type");
    }
}

}
