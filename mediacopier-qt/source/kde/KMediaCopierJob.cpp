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

#include "kde/KMediaCopierJob.hpp"
#include "worker.hpp"

#include <QApplication>

#include <QVariant>

KMediaCopierJob::KMediaCopierJob(
        Worker* worker,
        QString command,
        const std::filesystem::path& dstDir) :
    m_command{std::move(command)}, m_worker{worker}
{
    QApplication::setDesktopFileName("org.kde.dolphin");

    setCapabilities(Killable | Suspendable);
    setProgressUnit(Files);
    setProperty("destUrl", "file://" + QString::fromStdString(std::filesystem::absolute(dstDir)));
    setProperty("immediateProgressReporting", false);

    QObject::connect(m_worker, &Worker::updateDescription, this, &KMediaCopierJob::updateDescription);
    QObject::connect(m_worker, &Worker::updateProgress, this, &KMediaCopierJob::updateProgress);
    QObject::connect(m_worker, &Worker::finished, this, &KMediaCopierJob::quit);
}

void KMediaCopierJob::start()
{
    // nothing to do here
}

void KMediaCopierJob::updateDescription(StatusDescription info)
{
    description(this, m_command,
                qMakePair<QString, QString>(
                    tr("Source"), QString::fromStdString(info.inputPath)),
                qMakePair<QString, QString>(
                    tr("Destination"), QString::fromStdString(info.outputPath)));
}

void KMediaCopierJob::updateProgress(StatusProgress info)
{
    setTotalAmount(Files, info.count);
    setProcessedAmount(Files, info.progress);
}

void KMediaCopierJob::quit()
{
    emitResult();
}

bool KMediaCopierJob::doKill()
{
    m_worker->kill();
    return true;
}

bool KMediaCopierJob::doSuspend()
{
    m_worker->suspend();
    return true;
}

bool KMediaCopierJob::doResume()
{
    m_worker->resume();
    return true;
}
