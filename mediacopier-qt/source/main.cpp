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

#include "gui/MediaCopierDialogFull.hpp"
#include "gui/MediaCopierDialogSlim.hpp"
#include "worker.hpp"

#include <mediacopier/version.hpp>

#include <QApplication>
#include <QTranslator>

#include <spdlog/spdlog.h>

template <typename T>
int run_gui(QApplication& app, std::shared_ptr<Config> config)
{
    T dialog;
    dialog.init(config, app);
    dialog.show();
    return app.exec();
}

int run_cli(QApplication& app, std::shared_ptr<Config> config)
{
    Worker worker{*config};
    worker.start();
    return app.exec();
}

using UiFuncType = std::function<int(QApplication&, std::shared_ptr<Config>)>;

static const std::map<Config::UI, UiFuncType> uiFuncMap = {
    {Config::UI::FullGui, &run_gui<MediaCopierDialogFull>},
    {Config::UI::SlimGui, &run_gui<MediaCopierDialogSlim>},
    {Config::UI::NoGui, &run_cli},
};

int main(int argc, char *argv[])
{
    try {
        QApplication app(argc, argv);

        QTranslator translator;
        if (translator.load(":/translations/lang.qm"))
            app.installTranslator(&translator);

        app.setApplicationName(mediacopier::MEDIACOPIER_PROJECT_NAME);
        app.setApplicationVersion(mediacopier::MEDIACOPIER_VERSION);

        auto config = std::make_shared<Config>(app);
        return uiFuncMap.at(config->ui())(app, config);

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
