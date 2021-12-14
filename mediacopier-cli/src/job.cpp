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

MediaCopierJob::MediaCopierJob(
        std::shared_ptr<Worker> worker,
        const std::filesystem::path& dstDir) :
    m_worker{std::move(worker)}
{
    // reusing translations from kio
    KLocalizedString::setApplicationDomain("kio5");

    setCapabilities(Killable | Suspendable);
    setProgressUnit(Files);
    setProperty("destUrl", "file://" + QString::fromStdString(std::filesystem::absolute(dstDir)));
    setProperty("immediateProgressReporting", true);

    QObject::connect(m_worker.get(), &Worker::status, this, &MediaCopierJob::update);
    QObject::connect(m_worker.get(), &Worker::finished, this, &MediaCopierJob::quit);
}

void MediaCopierJob::start()
{
    // nothing to do here
}

void MediaCopierJob::update(Status info)
{
    description(this, i18n("Copy"),
                qMakePair<QString, QString>(
                    i18n("Source"), QString::fromStdString(info.inputPath())),
                qMakePair<QString, QString>(
                    i18n("Destination"), QString::fromStdString(info.outputPath())));
    setTotalAmount(Files, info.fileCount());
    setProcessedAmount(Files, info.progress());
}

void MediaCopierJob::quit()
{
    emitResult();
}

bool MediaCopierJob::doKill()
{
    m_worker->kill();
    return true;
}

bool MediaCopierJob::doSuspend()
{
    m_worker->suspend();
    return true;
}

bool MediaCopierJob::doResume()
{
    m_worker->resume();
    return true;
}
