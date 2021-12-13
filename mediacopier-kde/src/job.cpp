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

#include "job.hpp"

#include <KLocalizedString>

#include <QVariant>

MediaCopierJob::MediaCopierJob(const Worker::Command& command,
                               const std::filesystem::path& srcDir,
                               const std::filesystem::path& dstDir,
                               const std::string& pattern) :
    m_worker{command, srcDir, dstDir, pattern}
{
    setCapabilities(Killable | Suspendable);
    setProgressUnit(Files);
    setProperty("destUrl", "file://" + QString::fromStdString(std::filesystem::absolute(dstDir)));
    setProperty("immediateProgressReporting", command != Worker::Command::SHOW);

    qRegisterMetaType<Status>("Status");

    QObject::connect(&m_thread, &QThread::started, &m_worker, &Worker::initialize);
    QObject::connect(&m_worker, &Worker::initialized, &m_worker, &Worker::run);
    QObject::connect(&m_worker, &Worker::status, this, &MediaCopierJob::update);
    QObject::connect(&m_worker, &Worker::finished, &m_thread, &QThread::quit);
    QObject::connect(&m_thread, &QThread::finished, this, &MediaCopierJob::quit);

    m_worker.moveToThread(&m_thread);
}

void MediaCopierJob::start() {
    m_thread.start();
}

void MediaCopierJob::update(Status info) {
    description(this, i18n("Copy"),
                qMakePair<QString, QString>(
                    i18n("Source"), QString::fromStdString(info.inputPath())),
                qMakePair<QString, QString>(
                    i18n("Destination"), QString::fromStdString(info.outputPath())));
    setTotalAmount(Files, info.fileCount());
    setProcessedAmount(Files, info.progress());
}

void MediaCopierJob::quit() {
    m_thread.wait();
    emitResult();
}

bool MediaCopierJob::doKill() {
    auto res = m_worker.kill();
    /* shortcutting the regular shutdown (via signals / slots)
     * as object will be destroyed after 'doKill' was executed */
    m_thread.quit();
    m_thread.wait();
    return res;
}

bool MediaCopierJob::doSuspend() {
    return m_worker.suspend();
}

bool MediaCopierJob::doResume() {
    return m_worker.resume();
}
