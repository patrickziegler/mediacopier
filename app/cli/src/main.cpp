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

#include "config.hpp"

#include <mediacopier/abstract_file_info.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_operation_copy_jpeg.hpp>
#include <mediacopier/file_operation_move_jpeg.hpp>
#include <mediacopier/file_path_format.hpp>

#ifdef ENABLE_GUI
#include <mediacopier/gui/dialog.hpp>
#include <QApplication>
#endif

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
namespace mc = mediacopier;

using namespace mc::cli;

int run_cli(const Config& config)
{
    mc::FilePathFormat format{config.outputDir(), config.baseFormat()};

    std::unique_ptr<mc::AbstractFileOperation> op;

    switch (config.command()) {
    case Config::Command::COPY:
        op = std::make_unique<mc::FileOperationCopyJpeg>(format);
        break;

    case Config::Command::MOVE:
        op = std::make_unique<mc::FileOperationMoveJpeg>(format);
        break;

    default:
        // TODO: report this
        return 0;
    }

    mc::FileInfoFactory factory;

    for (const auto& path : fs::recursive_directory_iterator(config.inputDir())) {
        try {
            if (path.is_regular_file()) {
                auto file = factory.createFileFrom(path);
                file->accept(*op);
            }
        }  catch (const mc::FileInfoError& err) {
            std::cout << err.what() << " (" << path << ")" << std::endl;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    Config config;
    config.parseArgs(argc, argv);

#ifdef ENABLE_GUI
    if (config.command() == Config::Command::GUI) {
        QApplication app(argc, argv);
        MediaCopierDialog dialog;
        dialog.show();
        return app.exec();
    }
#endif
    return run_cli(config);
}
