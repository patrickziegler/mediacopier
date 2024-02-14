/* Copyright (C) 2021 Patrick Ziegler <zipat@proton.me>
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

#ifdef ENABLE_KDE
#include <kde/KMediaCopierJob.hpp>
#endif

#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_register.hpp>

#include <mediacopier/operation_copy_jpeg.hpp>
#include <mediacopier/operation_move_jpeg.hpp>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/iterator_range.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace fs = std::filesystem;

namespace {

namespace mc = mediacopier;

static volatile std::atomic<bool> operationCancelled(false);
static volatile std::atomic<bool> operationSuspended(false);

static constexpr const unsigned int DEFAULT_WAIT_MS = 200;

auto is_operation_cancelled()
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

auto media_files(const fs::path& path)
{
    using itemized_directory_entry = std::pair<size_t, const fs::directory_entry&>;
    using itemized_file_into_ptr = std::pair<size_t, mc::FileInfoPtr>;

    static auto itemize = [](const fs::directory_entry& entry) -> itemized_directory_entry {
        static size_t i = 0;
        ++i;
        return {i, entry};
    };

    static auto convert = [](const itemized_directory_entry& entry) -> itemized_file_into_ptr {
        if (fs::is_regular_file(entry.second)) {
            return {entry.first, mc::to_file_info_ptr(entry.second.path())};
        } else {
            return {entry.first, nullptr};
        }
    };

    return ranges::make_iterator_range(
                fs::recursive_directory_iterator(path),
                fs::recursive_directory_iterator())
            | ranges::views::transform(itemize)
            | ranges::views::transform(convert);
}

auto directory_entries_count(const fs::path& path) -> size_t
{
    return ranges::distance(
                ranges::make_iterator_range(
                    fs::recursive_directory_iterator(path),
                    fs::recursive_directory_iterator()));
}

template <typename Operation>
auto execute(const fs::path& dest, mc::FileInfoPtr file) -> void
{
    Operation op(dest);
    file->accept(op);
}

typedef void (*ExecFuncPtr)(const fs::path&, mc::FileInfoPtr);

} // namespace

Worker::Worker(Config config) : m_config{std::move(config)}
{
    qRegisterMetaType<Status>("Status");

    QObject::connect(&m_thread, &QThread::started, this, &Worker::exec);
    QObject::connect(this, &Worker::execDone, &m_thread, &QThread::quit);
    QObject::connect(&m_thread, &QThread::finished, this, &Worker::quit);

    this->moveToThread(&m_thread);

#ifdef ENABLE_KDE
    // tracker takes ownership of job
    auto job = new KMediaCopierJob(this, m_config.outputDir());
    m_tracker.registerJob(job);
#endif

    if (spdlog::default_logger()->sinks().size() > 2) {
        spdlog::default_logger()->sinks().pop_back();
    }

    auto logfile = m_config.outputDir() / ".mediacopier-log";
    spdlog::default_logger()->sinks().push_back(
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile.string(), true));
};

void Worker::start()
{
    m_thread.start();
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

void Worker::kill()
{
    spdlog::info("Cancelling operation..");
    operationCancelled.store(true);

    // block until request was recognized
    while(operationCancelled.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_WAIT_MS));
    }
}

void Worker::exec()
{
    ExecFuncPtr execute;
    switch (m_config.command()) {
    case Config::Command::COPY:
        execute = &::execute<mc::FileOperationCopyJpeg>;
        break;
    case Config::Command::MOVE:
        execute = &::execute<mc::FileOperationMoveJpeg>;
        break;
    }

    spdlog::info("Checking input directory..");
    const auto count = directory_entries_count(m_config.inputDir());

    // register callback for graceful shutdown via CTRL-C
    std::signal(SIGINT, [](int) -> void {
        operationCancelled.store(true);
    });

    auto fileRegister = mc::FileRegister{m_config.outputDir(), m_config.pattern()};
    std::optional<fs::path> dest;

    for (auto [progress, file] : media_files(m_config.inputDir())) {
        if (is_operation_cancelled()) {
            spdlog::info("Operation was cancelled..");
            break;
        }
        try {
            if (file != nullptr && (dest = fileRegister.add(file)).has_value()) {
                Q_EMIT status({m_config.command(), file->path(), dest.value(), count, progress});
                execute(dest.value(), file);
            }
        } catch (const std::exception& err) {
            spdlog::error(err.what());
        }
    }

    spdlog::info("Removing duplicates in destination directory..");
    fileRegister.removeDuplicates();

    spdlog::info("Writing config..");
    m_config.writeConfigFile();

    spdlog::info("Done");
    std::signal(SIGINT, SIG_DFL);
    Q_EMIT execDone();
}

void Worker::quit()
{
    Q_EMIT finished();
}
