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

#include <KUiServerV2JobTracker>
#include <QApplication>
#include <QFileDialog>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setDesktopFileName("org.kde.dolphin");

    QString dir = QFileDialog::getExistingDirectory(
                0, "Open Directory", QDir::currentPath(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    auto job = new MediaCopierJob{Worker::Command::SHOW,
            "/home/patrick/workspace/tools/tmp/",
            dir.toStdString()};

    QObject::connect(job, &MediaCopierJob::finished, &app, &QGuiApplication::quit);

    KUiServerV2JobTracker tracker;
    tracker.registerJob(job); // takes ownership

    job->start();

    return app.exec();
}
