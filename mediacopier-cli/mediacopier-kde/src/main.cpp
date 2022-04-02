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

#include "ui/dialog_kde.hpp"

#include <mediacopier/version.hpp>

#include <QApplication>
#include <QTranslator>

#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    try {
        Q_INIT_RESOURCE(common);

        QApplication app(argc, argv);

        QTranslator translator;
        if (translator.load(":/translations/lang.qm"))
            app.installTranslator(&translator);

        QTranslator translator_common;
        if (translator_common.load(":/translations/lang-common.qm"))
            app.installTranslator(&translator_common);

        app.setApplicationName(mediacopier::MEDIACOPIER_PROJECT_NAME);
        app.setApplicationVersion(mediacopier::MEDIACOPIER_VERSION);

        auto config = std::make_shared<Config>(app);

        KMediaCopierDialog dialog;
        dialog.init(config, app);
        dialog.show();

        return app.exec();

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
