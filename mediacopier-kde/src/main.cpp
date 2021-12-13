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

#include <mediacopier/version.hpp>

#include <spdlog/spdlog.h>

#include <KLocalizedString>
#include <KUiServerV2JobTracker>

#include <QApplication>
#include <QCommandLineParser>
#include <QFileDialog>

#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    try {
        QApplication app(argc, argv);

        // reusing translations from kio
        KLocalizedString::setApplicationDomain("kio5");

        app.setApplicationName(mediacopier::MEDIACOPIER_PROJECT_NAME);
        app.setApplicationVersion(mediacopier::MEDIACOPIER_VERSION);
        app.setDesktopFileName("org.kde.dolphin");

        QCommandLineParser parser;

        parser.setApplicationDescription(
                    app.applicationName() +
                    ", Copyright (C) 2020-2021 Patrick Ziegler");

        parser.addPositionalArgument(
                    "SRC", "Input directory", "[SRC");

        parser.addPositionalArgument(
                    "DST", "Output directory", "[DST]]");

        QCommandLineOption optCommand(
                    "c", "Available commands: copy (default), move, show",
                    "command", "copy");

        QCommandLineOption optPattern(
                    "f", "Base format to be used for new filenames",
                    "pattern", "%Y/%W/IMG_%Y%m%d_%H%M%S");

        parser.addOptions({optCommand, optPattern});
        parser.addVersionOption();
        parser.addHelpOption();
        parser.process(app);

        auto optValueCommand = parser.value("c").toLower();
        auto optValuePattern = parser.value("f").toStdString();

        auto askForDirectory = [](QString title) {
            auto dir = QFileDialog::getExistingDirectory(
                        0, title, QDir::currentPath(),
                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            if (dir.isEmpty())
                throw std::runtime_error("Aborted by user");

            return dir.toStdString();
        };

        fs::path inputDir, outputDir;

        if (parser.positionalArguments().length() > 0)
            inputDir = parser.positionalArguments().at(0).toStdString();
        else
            inputDir = askForDirectory(i18n("Source"));

        if (parser.positionalArguments().length() > 1)
            outputDir = parser.positionalArguments().at(1).toStdString();
        else
            outputDir = askForDirectory(i18n("Destination"));

        Worker::Command command;

        if (optValueCommand == "copy")
            command = Worker::Command::COPY_JPEG;
        else if (optValueCommand == "move")
            command = Worker::Command::MOVE_JPEG;
        else if (optValueCommand == "show")
            command = Worker::Command::SHOW;

        auto job = new MediaCopierJob{command, inputDir, outputDir, optValuePattern};

        QObject::connect(job, &MediaCopierJob::finished, &app, &QGuiApplication::quit);

        KUiServerV2JobTracker tracker;
        tracker.registerJob(job); // takes ownership of job

        job->start();

        return app.exec();

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
