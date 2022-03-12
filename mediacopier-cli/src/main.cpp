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

#include "core/worker.hpp"
#include "gui/dialog.hpp"

#include <mediacopier/version.hpp>

#ifdef ENABLE_KDE
#include "kde/mediacopierjob.hpp"
#include <KUiServerV2JobTracker>
#endif

#include <QApplication>
#include <QTranslator>

int runCli(QApplication& app, Config& config)
{
    auto worker = std::make_shared<Worker>(config);
    QObject::connect(worker.get(), &Worker::finished, &app, &QGuiApplication::quit);

#ifdef ENABLE_KDE
    app.setDesktopFileName("org.kde.dolphin");
    KUiServerV2JobTracker tracker;
    // tracker takes ownership of new MediaCopierJob
    tracker.registerJob(new MediaCopierJob(worker, config.outputDir()));
#endif

    worker->start();
    return app.exec();
}

int runGui(QApplication& app, Config& config)
{
    spdlog::info("Using graphical user interface");
    MediaCopierDialog dialog{&config};
    dialog.show();
    return app.exec();
}

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

        Config config{app};

        if (!config.useGui())
            return runCli(app, config);
        else
            return runGui(app, config);

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
