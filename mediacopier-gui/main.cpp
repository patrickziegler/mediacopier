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

#include "dialog.hpp"
#include "worker.hpp"

#include <mediacopier/version.hpp>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(mediacopier::MEDIACOPIER_PROJECT_NAME);
    QCoreApplication::setApplicationVersion(mediacopier::MEDIACOPIER_VERSION);

    QCommandLineParser parser;

    parser.setApplicationDescription(
                QCoreApplication::applicationName() +
                ", Copyright (C) 2020-2021 Patrick Ziegler");

    parser.addPositionalArgument(
                "SRC", "Input directory", "[SRC");

    parser.addPositionalArgument(
                "DST", "Output directory", "[DST]]");

    QCommandLineOption optCommand(
                "c", "Available commands: copy (default), move, sim", "command", "copy");

    QCommandLineOption optPattern(
                "f", "Base format to be used for new filenames", "pattern", "%Y/%u/IMG_%Y%m%d_%H%M%S");

    parser.addOptions({optCommand, optPattern});
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    Worker worker;

    if (parser.positionalArguments().length() > 0) {
        worker.m_inputDir = parser.positionalArguments().at(0);
    }

    if (parser.positionalArguments().length() > 1) {
        worker.m_outputDir = parser.positionalArguments().at(1);
    }

    worker.m_pattern = parser.value("f");

    auto cmd = parser.value("c").toLower();

    if (cmd == "copy") {
        worker.m_command = Worker::Command::COPY_JPEG;
    } else if (cmd == "move") {
        worker.m_command = Worker::Command::MOVE_JPEG;
    } else if (cmd == "sim") {
        worker.m_command = Worker::Command::SIMULATE;
    }

    MediaCopierDialog dialog(&worker);
    dialog.show();

    return app.exec();
}
