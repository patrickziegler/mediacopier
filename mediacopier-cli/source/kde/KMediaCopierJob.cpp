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

#include "kde/KMediaCopierJob.hpp"
#include "worker.hpp"

#include <QApplication>

#include <QVariant>

KMediaCopierJob::KMediaCopierJob(
        Worker* worker,
        const std::filesystem::path& dstDir) :
    m_worker{worker}
{
    QApplication::setDesktopFileName("org.kde.dolphin");

    setCapabilities(Killable | Suspendable);
    setProgressUnit(Files);
    setProperty("destUrl", "file://" + QString::fromStdString(std::filesystem::absolute(dstDir)));
    setProperty("immediateProgressReporting", false);

    QObject::connect(m_worker, &Worker::status, this, &KMediaCopierJob::update);
    QObject::connect(m_worker, &Worker::finished, this, &KMediaCopierJob::quit);
}

void KMediaCopierJob::start()
{
    // nothing to do here
}

void KMediaCopierJob::update(Status info)
{
    description(this, Config::commandString(info.command()),
                qMakePair<QString, QString>(
                    tr("Source"), QString::fromStdString(info.inputPath())),
                qMakePair<QString, QString>(
                    tr("Destination"), QString::fromStdString(info.outputPath())));
    setTotalAmount(Files, info.fileCount());
    setProcessedAmount(Files, info.progress());
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
