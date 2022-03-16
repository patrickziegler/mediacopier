/* Copyright (C) 2021-2022 Patrick Ziegler
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

    return make_iterator_range(
                fs::recursive_directory_iterator(path),
                fs::recursive_directory_iterator())
            | views::filter(is_regular_file)
            | views::transform(to_file_info_ptr)
            | views::filter(is_valid);
}

template <typename T>
auto create_executor()
{
    return [](const FileInfoPtr& file, const fs::path& path) {
        T op(path);
        file->accept(op);
    };
}

auto get_executor(Config::Command command)
{
    std::function<void(const FileInfoPtr&, const fs::path&)> executor;

    // dispatch necessary as Q_OBJECT does not allow templated classes
    switch(command) {

    case Config::Command::COPY:
        spdlog::info("Executing COPY operation..");
        executor = create_executor<FileOperationCopy>();
        break;

    case Config::Command::COPY_JPEG:
        spdlog::info("Executing COPY operation (jpeg aware)..");
        executor = create_executor<FileOperationCopyJpeg>();
        break;

    case Config::Command::MOVE:
        spdlog::info("Executing MOVE operation");
        executor = create_executor<FileOperationMove>();
        break;

    case Config::Command::MOVE_JPEG:
        spdlog::info("Executing MOVE operation (jpeg aware)..");
        executor = create_executor<FileOperationMoveJpeg>();
        break;

    case Config::Command::SHOW:
        spdlog::info("Executing SHOW operation..");
        executor = create_executor<FileOperationShow>();
        break;

    default:
        throw std::runtime_error("Unknown operation..");
    }

    return executor;
}

} // namespace mediacopier

Worker::Worker(Config config) : m_config{std::move(config)}
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

    // block until request was recognized
    while(operationCancelled.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_WAIT_MS));
    }
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
        auto executor = mc::get_executor(m_config.command());
        mc::FileRegister destRegister{m_config.outputDir(), m_config.pattern()};
        const auto cmd = Config::commandString(m_config.command());
        fs::path lastPath = "";
        size_t progress = 0;

        spdlog::info("Checking input folder..");
        size_t fileCount = ranges::distance(mc::valid_media_files(m_config.inputDir()));

        spdlog::info("Starting execution..");
        for (auto file : mc::valid_media_files(m_config.inputDir())) {
            auto path = destRegister.add(file);
            if (path.has_value()) {
                lastPath = path.value();
                Q_EMIT status({cmd, file->path(), lastPath, fileCount, progress});
                executor(file, lastPath);
            }
            if (check_operation_state()) {
                spdlog::warn("Operation was cancelled");
                break;
            }
            ++progress;
            Q_EMIT status({cmd, file->path(), lastPath, fileCount, progress});
        }

        spdlog::info("Writing config..");
        m_config.writeConfigFile();

        spdlog::info("Done");

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
