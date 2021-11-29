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

#include "worker.hpp"

#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_info_register.hpp>
#include <mediacopier/operations/move.hpp>
#include <mediacopier/operations/move_jpeg.hpp>
#include <mediacopier/operations/show.hpp>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/iterator_range.hpp>

#include <spdlog/spdlog.h>

#include <atomic>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;
namespace mc = mediacopier;

namespace {

static volatile std::atomic<bool> operationCancelled(false);
static volatile std::atomic<bool> operationSuspended(false);

static constexpr const unsigned int DEFAULT_WAIT_MS = 200;

auto check_operation_state()
{
    while (true) {
        if (operationCancelled.load()) {
            operationCancelled.store(false);
            return true;
        }
        if (operationSuspended.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_WAIT_MS));
            continue;
        }
        break;
    }
    return false;
}

} // namespace

namespace mediacopier {

auto valid_media_files(const fs::path& path)
{
    using namespace ranges;

    static auto is_regular_file = [](const fs::directory_entry& path) {
        return fs::is_regular_file(path);
    };

    static auto is_valid = [](const FileInfoPtr& file) {
        return file != nullptr;
    };

    static auto is_not_cancelled = [](const FileInfoPtr& file) {
        if (check_operation_state()) {
            throw std::runtime_error("Operation was cancelled");
        }
        return true;
    };

    return make_iterator_range(
                fs::recursive_directory_iterator(path),
                fs::recursive_directory_iterator())
            | views::filter(is_regular_file)
            | views::transform(to_file_info_ptr)
            | views::filter(is_valid)
            | views::filter(is_not_cancelled);
}

} // namespace mediacopier

Worker::Worker(const Command& command,
               fs::path inputDir,
               fs::path outputDir,
               std::string pattern) :
    m_command(command),
    m_inputDir{std::move(inputDir)},
    m_outputDir{std::move(outputDir)},
    m_pattern{std::move(pattern)}
{
    // nothing to do here
};

bool Worker::kill()
{
    spdlog::info("Cancelling operation..");
    operationCancelled.store(true);
    while(operationCancelled.load()) {
        // blocking until request was recognized
        std::this_thread::sleep_for(
                    std::chrono::milliseconds(DEFAULT_WAIT_MS));
    }
    return true;
}

bool Worker::suspend()
{
    spdlog::info("Pausing operation..");
    operationSuspended.store(true);
    return true;
}

bool Worker::resume()
{
    spdlog::info("Resuming operation..");
    operationSuspended.store(false);
    return true;
}

template <typename T>
void Worker::execute()
{
    mc::FileRegister destRegister{m_outputDir, m_pattern};
    size_t progress = 1;

    for (auto file : mc::valid_media_files(m_inputDir)) {

        auto path = destRegister.add(file);

        if (path.has_value()) {
            Q_EMIT status({file->path(), path.value(), m_fileCount, progress});

            // operation visits the file here
            T op(path.value());
            file->accept(op);
        }

        ++progress;
    }
}

void Worker::initialize()
{
    try {
        spdlog::info("Initializing worker..");
        m_fileCount = ranges::distance(mc::valid_media_files(m_inputDir));
        Q_EMIT initialized();
    } catch (const std::exception& err) {
        spdlog::error(err.what());
    }
}

void Worker::run()
{
    try {
        // dispatch necessary as Q_OBJECT does not allow templated classes
        switch(m_command) {

        case Command::COPY:
            spdlog::info("Executing COPY operation..");
            execute<mc::FileOperationCopy>();
            break;

        case Command::COPY_JPEG:
            spdlog::info("Executing COPY operation (jpeg aware)..");
            execute<mc::FileOperationCopyJpeg>();
            break;

        case Command::MOVE:
            spdlog::info("Executing MOVE operation");
            execute<mc::FileOperationMove>();
            break;

        case Command::MOVE_JPEG:
            spdlog::info("Executing MOVE operation (jpeg aware)..");
            execute<mc::FileOperationMoveJpeg>();
            break;

        case Command::SHOW:
            spdlog::info("Executing SHOW operation..");
            execute<mc::FileOperationShow>();
            break;

        default:
            spdlog::error("Unknown operation..");
        }

        Q_EMIT finished();

    } catch (const std::exception& err) {
        spdlog::error(err.what());
    }
}
