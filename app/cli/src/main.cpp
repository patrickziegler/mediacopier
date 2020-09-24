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

#include <mediacopier/abstract_file_info.hpp>
#include <mediacopier/exceptions.hpp>
#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_operation_copy_jpeg.hpp>
#include <mediacopier/file_operation_move_jpeg.hpp>
#include <mediacopier/file_path_format.hpp>

#include <mediacopier/gui/mediacopierdialog.hpp>
#include <QApplication>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
namespace mc = MediaCopier;

int demo_test()
{
    fs::path src{"/home/patrick/workspace/repos/tmp/"};
    fs::path dst{"/home/patrick/workspace/repos/tmp_out/"};

    mc::FilePathFormat fmt{dst, "TEST_%Y%m%d_%H%M%S_"};
    std::unique_ptr<mc::AbstractFileOperation> op;

    bool move = false;

    if (move) {
        op = std::make_unique<mc::FileOperationMoveJpeg>(fmt);
    } else {
        op = std::make_unique<mc::FileOperationCopyJpeg>(fmt);
    }

    mc::FileInfoFactory factory;

    for (const auto& path : fs::recursive_directory_iterator(src)) {
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
    QApplication app(argc, argv);
    MediaCopierDialog dialog;
    dialog.show();
    return app.exec();
}
