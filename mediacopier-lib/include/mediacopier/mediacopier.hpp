/* Copyright (C) 2022 Patrick Ziegler
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

#include <mediacopier/operation_copy.hpp>
#include <mediacopier/operation_copy_jpeg.hpp>
#include <mediacopier/operation_move.hpp>
#include <mediacopier/operation_move_jpeg.hpp>
#include <mediacopier/operation_simulate.hpp>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/iterator_range.hpp>

#include <spdlog/spdlog.h>

namespace mediacopier {

namespace fs = std::filesystem;

auto valid_media_files(const fs::path& path)
{
    using namespace ranges;

    static auto is_regular_file = [](const fs::directory_entry& path) {
        return fs::is_regular_file(path);
    };

    static auto is_valid = [](const FileInfoPtr& file) {
        return file != nullptr;
    };

    return make_iterator_range(
                fs::recursive_directory_iterator(path),
                fs::recursive_directory_iterator())
            | views::filter(is_regular_file)
            | views::transform(to_file_info_ptr)
            | views::filter(is_valid);
}

auto valid_media_file_count(const fs::path& inputDir) -> size_t
{
    return ranges::distance(valid_media_files(inputDir));
}

template <typename T>
auto execute(
        const fs::path& inputDir,
        const fs::path& outputDir,
        const std::string& pattern,
        std::function<void(const fs::path&, const fs::path&)> callbackStatus)
{
    auto destRegister = std::make_unique<FileRegister>(outputDir, pattern);
    fs::path lastPath = "";

    try {
        for (auto file : valid_media_files(inputDir)) {
            auto destPath = destRegister->add(file);

            if (destPath.has_value()) {
                lastPath = destPath.value();
                T op(destPath.value());
                file->accept(op);
            }

            callbackStatus(file->path(), lastPath);
        }

    } catch (const std::exception& err) {
        spdlog::error(err.what());
    }

    return std::move(destRegister);
}

} // namespace mediacopier
