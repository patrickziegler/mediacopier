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

#include "core.hpp"
#include "worker.hpp"

#ifdef ENABLE_KDE
#include <kde/KMediaCopierJob.hpp>
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace fs = std::filesystem;

namespace {

using ExecFuncType = std::function<std::unique_ptr<mc::FileRegister>(const fs::path&, const fs::path&, const std::string&, std::function<void(const fs::path&, const fs::path&)>)>;

static const std::map<Config::Command, ExecFuncType> execFuncMap = {
    {Config::Command::COPY, &execute<mc::FileOperationCopy>},
    {Config::Command::COPY_JPEG, &execute<mc::FileOperationCopyJpeg>},
    {Config::Command::MOVE, &execute<mc::FileOperationMove>},
    {Config::Command::MOVE_JPEG, &execute<mc::FileOperationMoveJpeg>},
    {Config::Command::SIMULATE, &execute<mc::FileOperationSimulate>}};

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
    // register callback for graceful shutdown via CTRL-C
    std::signal(SIGINT, [](int) -> void {
        operationCancelled.store(true);
    });

    m_progress = 1;

    // create callback for status update
    auto callbackStatus = [this](const fs::path& src, const fs::path& dst) {
        if (check_operation_state()) {
            throw std::runtime_error("Operation was cancelled..");
        }
        Q_EMIT status({m_config.command(), src, dst, m_fileCount, m_progress});
        ++m_progress;
    };

    spdlog::info("Checking input directory..");
    m_fileCount = valid_media_file_count(m_config.inputDir());

    auto reg = execFuncMap.at(m_config.command())(
                m_config.inputDir(), m_config.outputDir(),
                m_config.pattern(), callbackStatus);

    spdlog::info("Removing duplicates..");
    reg->removeDuplicates();

    spdlog::info("Writing config..");
    m_config.writeConfigFile();

    std::signal(SIGINT, SIG_DFL);

    Q_EMIT execDone();
}

void Worker::quit()
{
    Q_EMIT finished();
}
