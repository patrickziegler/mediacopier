/* Copyright (C) 2020 Patrick Ziegler
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
#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>
#include <mediacopier/FilePathFactory.hpp>
#include <mediacopier/gui/worker.hpp>

#include <chrono>
#include <filesystem>

namespace mc  = MediaCopier;
namespace cli = MediaCopier::Cli;
namespace fs  = std::filesystem;

static constexpr const size_t PROGRESS_UPDATE_INTERVAL_MS = 500;

void Worker::useConfig(cli::ConfigStore config)
{
    m_config = std::move(config);
}

void Worker::run()
{
    try {
        m_operationCancelled = false;

        mc::FilePathFactory filePathFactory{m_config.outputDir(), m_config.baseFormat()};
        std::unique_ptr<mc::AbstractFileOperation> op{};

        size_t count = 0;

        count = std::distance(
                    fs::recursive_directory_iterator(m_config.inputDir()),
                    fs::recursive_directory_iterator{});

        switch (m_config.command())
        {
        case cli::ConfigStore::Command::COPY:
            op = std::make_unique<mc::FileOperationCopyJpeg>(filePathFactory);
            emit info(QString{"Copying %1 files"}.arg(count));
            break;

        case cli::ConfigStore::Command::MOVE:
            op = std::make_unique<mc::FileOperationMoveJpeg>(filePathFactory);
            emit info(QString{"Moving %1 files"}.arg(count));
            break;

        default:
            emit error("Unknown operation type");
            emit finished();
            return;
        }

        emit progress(0);

        auto clock = std::chrono::steady_clock();
        auto ref = clock.now();

        size_t diff = 0;
        size_t pos = 0;

        MediaCopier::FileInfoFactory fileInfoFactory{};

        for (const auto& path : fs::recursive_directory_iterator(m_config.inputDir())) {
            try {
                if (path.is_regular_file()) {
                    auto file = fileInfoFactory.createFromPath(path);
                    file->accept(*op);
                }
            }  catch (const mc::FileInfoError& err) {
                emit warning(QString{"%1 (%2)"}.arg(err.what(), path.path().c_str()));
            }

            ++pos;

            diff = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - ref).count();

            if (diff > PROGRESS_UPDATE_INTERVAL_MS) {
                emit progress(100 * pos / count);
                ref = clock.now();
            }

            if (m_operationCancelled) {
                emit info("Operation cancelled");
                break;
            }
        }

    } catch (const std::exception& err) {
        emit error(err.what());
    }

    emit finished();
}

void Worker::cancel()
{
    m_operationCancelled = true;
}
