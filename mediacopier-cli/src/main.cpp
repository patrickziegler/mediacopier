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

#include <mediacopier/version.hpp>

#include <spdlog/spdlog.h>

#ifdef ENABLE_KDE
#include "job.hpp"
#include <KUiServerV2JobTracker>
#endif

#include <QApplication>
#include <QTranslator>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    try {
        QApplication app(argc, argv);

        QTranslator translator;

        if (translator.load(":/translations/lang.qm"))
            app.installTranslator(&translator);
        else if (!QLocale::system().name().startsWith("en"))
            spdlog::warn("Error loading the translation");

        app.setApplicationName(mediacopier::MEDIACOPIER_PROJECT_NAME);
        app.setApplicationVersion(mediacopier::MEDIACOPIER_VERSION);
        app.setDesktopFileName("org.kde.dolphin");

        Config config{app};

        auto worker = std::make_shared<Worker>(config);

        QObject::connect(worker.get(), &Worker::finished, &app, &QGuiApplication::quit);

#ifdef ENABLE_KDE
        KUiServerV2JobTracker tracker;
        tracker.registerJob(new MediaCopierJob(worker, config.outputDir())); // takes ownership of job
#endif
        worker->start();

        return app.exec();

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
