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

#include "config.hpp"

#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_info_register.hpp>
#include <mediacopier/operations/move.hpp>
#include <mediacopier/operations/move_jpeg.hpp>
#include <mediacopier/operations/show.hpp>

#include <range/v3/iterator_range.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>

namespace fs = std::filesystem;

static volatile std::atomic<bool> operationCancelled(false);

namespace mediacopier {

static auto valid_media_files(const fs::path& dir)
{
    using namespace ranges;

    static auto is_regular_file = [](const fs::directory_entry& path) {
        return fs::is_regular_file(path);
    };

    static auto is_valid = [](const FileInfoPtr& file) {
        return file != nullptr;
    };

    return make_iterator_range(
                fs::recursive_directory_iterator(dir),
                fs::recursive_directory_iterator())
            | views::filter(is_regular_file)
            | views::transform(to_file_info_ptr)
            | views::filter(is_valid);
}

template<typename T>
static auto execute(const Config& config)
{
    // register callback for graceful shutdown via CTRL-C
    std::signal(SIGINT, [](int) -> void {
        operationCancelled.store(true);
    });

    FileRegister destinationRegister{config.outputDir, config.pattern};

    for (auto file : valid_media_files(config.inputDir)) {

        auto destPath = destinationRegister.add(file);

        if (destPath.has_value()) {
            T operation(destPath.value());
            file->accept(operation);
        }

        if (operationCancelled.load()) {
            break;
        }
    }

    std::signal(SIGINT, SIG_DFL);
    return 0;
}

} // namespace mediacopier

int main(int argc, char *argv[])
{
    using namespace mediacopier;

    try {
        Config config{argc, argv};

        switch(config.command) {

        case Config::Command::COPY:
            spdlog::info("Executing COPY operation");
            return execute<FileOperationCopy>(config);

        case Config::Command::COPY_JPEG:
            spdlog::info("Executing COPY operation (jpeg aware)");
            return execute<FileOperationCopyJpeg>(config);

        case Config::Command::MOVE:
            spdlog::info("Executing MOVE operation");
            return execute<FileOperationMove>(config);

        case Config::Command::MOVE_JPEG:
            spdlog::info("Executing MOVE operation (jpeg aware)");
            return execute<FileOperationMoveJpeg>(config);

        case Config::Command::SHOW:
            spdlog::info("Executing SHOW operation");
            return execute<FileOperationShow>(config);
        }

    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }
}
