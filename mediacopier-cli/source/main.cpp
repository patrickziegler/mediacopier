/* Copyright (C) 2023 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_register.hpp>
#include <mediacopier/operation_copy_jpeg.hpp>
#include <mediacopier/operation_move_jpeg.hpp>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/iterator_range.hpp>

#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>

#include "config.hpp"

namespace fs = std::filesystem;
namespace mc = mediacopier;

static volatile std::atomic<bool> operationCancelled(false);

auto valid_media_files(const fs::path& path)
{
    static auto is_regular_file = [](const fs::directory_entry& path) {
        return fs::is_regular_file(path);
    };
    static auto is_valid = [](const mc::FileInfoPtr& file) {
        return file != nullptr;
    };
    return ranges::make_iterator_range(
                fs::recursive_directory_iterator(path),
                fs::recursive_directory_iterator())
            | ranges::views::filter(is_regular_file)
            | ranges::views::transform(mc::to_file_info_ptr)
            | ranges::views::filter(is_valid);
}

template <typename Operation>
auto exec(const Config& config) -> void
{
    // register callback for graceful shutdown via CTRL-C
    std::signal(SIGINT, [](int) -> void {
        operationCancelled.store(true);
    });

    spdlog::info("Checking input directory..");
    auto fileRegister = mc::FileRegister{config.outputDir, config.pattern};
    std::optional<fs::path> dest;

    for (auto file : valid_media_files(config.inputDir)) {
        if (operationCancelled.load()) {
            spdlog::warn("Operation was cancelled..");
            break;
        }
        try {
            if ((dest = fileRegister.add(file)).has_value()) {
                spdlog::info("Processing: {} -> {}",
                             file->path().string(),
                             dest.value().string());
                Operation op(dest.value());
                file->accept(op);
            }
        } catch (const std::exception& err) {
            spdlog::error(err.what());
        }
    }

    spdlog::info("Removing duplicates in destination directory..");
    fileRegister.removeDuplicates();

    spdlog::info("Done");
    std::signal(SIGINT, SIG_DFL);
}

int main(int argc, char *argv[])
{
    Config config;
    const auto& [res, ret] = config.parseArgs(argc, argv);
    if (res != Config::ParseResult::Continue) {
        return ret;
    }
    config.loadPersistentConfig();
    config.finalize();

    switch (config.cmd) {
    case Config::Command::Copy:
        exec<mediacopier::FileOperationCopyJpeg>(config);
        break;
    case Config::Command::Move:
        exec<mediacopier::FileOperationMoveJpeg>(config);
        break;
    }

    config.storePersistentConfig();
    return 0;
}
