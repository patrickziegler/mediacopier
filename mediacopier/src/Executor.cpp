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

#include <mediacopier/AbstractFileInfo.hpp>
#include <mediacopier/Error.hpp>
#include <mediacopier/Executor.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FileRegister.hpp>

namespace fs = std::filesystem;

#include <atomic>
#include <csignal>
#include <functional>

static volatile std::atomic<bool> operationCancelled;

template <typename T>
static T abortable_wrapper(std::function<T()> fn)
{
    operationCancelled.store(false);
    std::signal(SIGINT, [](int signal) -> void {
        (void) signal;
        operationCancelled.store(true);
    });
    T result = fn();
    std::signal(SIGINT, SIG_DFL);
    return result;
}

template <typename T>
static void execute(const MediaCopier::FileRegister& fileRegister)
{
    for (const auto& item : fileRegister) {
        T operation{item.first};
        item.second->accept(operation);
        if (operationCancelled.load()) {
            break;
        }
    }
}

namespace MediaCopier {

void Executor::run()
{
    if (!fs::is_directory(m_inputDir)) {
        throw MediaCopierError("Input folder does not exist");
    }

    FileRegister fileRegister{m_outputDir, m_pattern};

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
        throw MediaCopierError("No files were found in " + m_inputDir.string());
    }

    switch (m_command)
    {
    case Command::COPY:
        abortable_wrapper<int>([fileRegister]() -> int {
            execute<FileOperationCopyJpeg>(fileRegister);
            return 0;
        });
        break;

    case Command::MOVE:
        abortable_wrapper<int>([fileRegister]() -> int {
            execute<FileOperationMoveJpeg>(fileRegister);
            return 0;
        });
        break;

    default:
        throw FileOperationError("Unknown operation type");
    }
}

} // namespace MediaCopier
