/* Copyright (C) 2020 Patrick Ziegler
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

#include <mediacopier/cli/ConfigStore.hpp>
#include <mediacopier/cli/execute.hpp>

#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

#ifdef ENABLE_GUI
#include <mediacopier/gui/dialog.hpp>
#include <QApplication>
#endif

namespace fs = std::filesystem;

using namespace MediaCopier::CLI;

int main(int argc, char *argv[])
{
    log4cplus::BasicConfigurator log;
    log.configure();

    ConfigStore config;
    config.parseArgs(argc, argv);

#ifdef ENABLE_GUI
    if (config.command() == ConfigStore::Command::GUI) {
        QApplication app(argc, argv);
        MediaCopierDialog dialog;
        dialog.show();
        return app.exec();
    }
#endif
    return execute(config);
}
