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
#include <csignal>
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

template <typename T>
auto executor()
{
    return [](const FileInfoPtr& file, const fs::path& path) {
        T op(path);
        file->accept(op);
    };
}

auto create_executor(Worker::Command command)
{
    std::function<void(const FileInfoPtr&, const fs::path&)> _exec;

    // dispatch necessary as Q_OBJECT does not allow templated classes
    switch(command) {

    case Worker::Command::COPY:
        spdlog::info("Executing COPY operation..");
        _exec = executor<FileOperationCopy>();
        break;

    case Worker::Command::COPY_JPEG:
        spdlog::info("Executing COPY operation (jpeg aware)..");
        _exec = executor<FileOperationCopyJpeg>();
        break;

    case Worker::Command::MOVE:
        spdlog::info("Executing MOVE operation");
        _exec = executor<FileOperationMove>();
        break;

    case Worker::Command::MOVE_JPEG:
        spdlog::info("Executing MOVE operation (jpeg aware)..");
        _exec = executor<FileOperationMoveJpeg>();
        break;

    case Worker::Command::SHOW:
        spdlog::info("Executing SHOW operation..");
        _exec = executor<FileOperationShow>();
        break;

    default:
        throw std::runtime_error("Unknown operation..");
    }

    return _exec;
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
    qRegisterMetaType<Status>("Status");

    QObject::connect(&m_thread, &QThread::started, this, &Worker::exec);
    QObject::connect(this, &Worker::execDone, &m_thread, &QThread::quit);
    QObject::connect(&m_thread, &QThread::finished, this, &Worker::quit);

    this->moveToThread(&m_thread);
};

void Worker::start()
{
    m_thread.start();
}

void Worker::kill()
{
    spdlog::info("Cancelling operation..");
    operationCancelled.store(true);
    while(operationCancelled.load()) {
        // blocking until request was recognized
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_WAIT_MS));
    }
    m_thread.quit();
    m_thread.wait();
}

void Worker::suspend()
{
    spdlog::info("Pausing operation..");
    operationSuspended.store(true);
}

void Worker::resume()
{
    spdlog::info("Resuming operation..");
    operationSuspended.store(false);
}

void Worker::exec()
{
    // register callback for graceful shutdown via CTRL-C
    std::signal(SIGINT, [](int) -> void {
        operationCancelled.store(true);
    });

    try {
        auto _exec = mc::create_executor(m_command);

        mc::FileRegister destRegister{m_outputDir, m_pattern};
        size_t progress = 1;

        spdlog::info("Checking input folder..");
        size_t fileCount = ranges::distance(mc::valid_media_files(m_inputDir));

        spdlog::info("Starting execution..");
        for (auto file : mc::valid_media_files(m_inputDir)) {
            auto path = destRegister.add(file);
            if (path.has_value()) {
                Q_EMIT status({file->path(), path.value(), fileCount, progress});
                _exec(file, path.value());
            }
            ++progress;
        }

    } catch (const std::exception& err) {
        spdlog::error(err.what());
    }

    std::signal(SIGINT, SIG_DFL);

    Q_EMIT execDone();
}

void Worker::quit()
{
    Q_EMIT finished();
}