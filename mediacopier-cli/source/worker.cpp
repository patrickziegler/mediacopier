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
#include "core.hpp"

#include <mediacopier/operation_move.hpp>
#include <mediacopier/operation_move_jpeg.hpp>
#include <mediacopier/operation_simulate.hpp>

#ifdef ENABLE_KDE
#include "kde/KMediaCopierJob.hpp"
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace fs = std::filesystem;

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

Worker::Worker(Config config) : m_config{std::move(config)}
{
    qRegisterMetaType<Status>("Status");

    QObject::connect(&m_thread, &QThread::started, this, &Worker::exec);
    QObject::connect(this, &Worker::execDone, &m_thread, &QThread::quit);
    QObject::connect(&m_thread, &QThread::finished, this, &Worker::quit);

    this->moveToThread(&m_thread);

#ifdef ENABLE_KDE
    auto job = new KMediaCopierJob(this, m_config.outputDir());
    m_tracker.registerJob(job); // tracker takes ownership of job
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

template <typename T>
void execute(const Config& m_config, std::function<void(const fs::path&, const fs::path)> cb)
{
    spdlog::info("Executing operation..");
    auto reg = mediacopier::execute<T>(
                m_config.inputDir(),
                m_config.outputDir(),
                m_config.pattern(),
                cb);

    spdlog::info("Removing duplicates..");
    reg->removeDuplicates();
}

void Worker::exec()
{
    // register callback for graceful shutdown via CTRL-C
    std::signal(SIGINT, [](int) -> void {
        operationCancelled.store(true);
    });

    // create callback for status update
    auto callbackStatus = [this](const fs::path& src, const fs::path& dst) {
        if (check_operation_state()) {
            throw std::runtime_error("Operation was cancelled..");
        }
        Q_EMIT status({m_config.command(), src, dst, 0, 0});
    };

    switch(m_config.command()) {

    case Config::Command::COPY:
        spdlog::info("Executing COPY operation..");
        execute<mediacopier::FileOperationCopy>(m_config, callbackStatus);
        break;

    case Config::Command::COPY_JPEG:
        spdlog::info("Executing COPY operation (jpeg aware)..");
        execute<mediacopier::FileOperationCopyJpeg>(m_config, callbackStatus);
        break;

    case Config::Command::MOVE:
        spdlog::info("Executing MOVE operation");
        execute<mediacopier::FileOperationMove>(m_config, callbackStatus);
        break;

    case Config::Command::MOVE_JPEG:
        spdlog::info("Executing MOVE operation (jpeg aware)..");
        execute<mediacopier::FileOperationMoveJpeg>(m_config, callbackStatus);
        break;

    case Config::Command::SIMULATE:
        spdlog::info("Executing SIMULATE operation..");
        execute<mediacopier::FileOperationSimulate>(m_config, callbackStatus);
        break;

    default:
        throw std::runtime_error("Unknown operation..");
    }

    spdlog::info("Writing config..");
    m_config.writeConfigFile();

    std::signal(SIGINT, SIG_DFL);

    Q_EMIT execDone();
}

void Worker::quit()
{
    Q_EMIT finished();
}
