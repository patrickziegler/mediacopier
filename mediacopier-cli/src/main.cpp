/* Copyright (C) 2020-2021 Patrick Ziegler
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

#include "cli/SequentialExecutor.hpp"

#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    try {
        MediaCopier::Cli::SequentialExecutor executor{{argc, argv}};
        executor.run();
    } catch (const std::exception& err) {
        spdlog::error(err.what());
        return 1;
    }

    return 0;
}
