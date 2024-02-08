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

#include "config.hpp"

#include <CLI/CLI.hpp>

#include <spdlog/spdlog.h>
#include <toml.hpp>

#include <mediacopier/version.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

constexpr static const char* DEFAULT_PATTERN = "%Y/%W/IMG_%Y%m%d_%H%M%S";
constexpr static const char* PERSISTENT_CONFIG = ".mediacopier";

std::pair<Config::ParseResult, int> Config::parseArgs(int argc, char *argv[])
{
    CLI::App app{mediacopier::MEDIACOPIER_PROJECT_NAME};
    app.set_version_flag("-v,--version", mediacopier::MEDIACOPIER_VERSION);

    auto copyapp = app.add_subcommand("copy", "Copy some files");
    copyapp->callback([this]() { cmd = Command::Copy; });
    copyapp->add_option("inputDir", inputDir)->required()->check(CLI::ExistingDirectory);
    copyapp->add_option("outputDir", outputDir)->required();
    copyapp->add_option("-p,--pattern", pattern);

    auto moveapp = app.add_subcommand("move", "Move some files");
    moveapp->callback([this]() { cmd = Command::Move; });
    moveapp->add_option("inputDir", inputDir)->required()->check(CLI::ExistingDirectory);
    moveapp->add_option("outputDir", outputDir)->required();
    moveapp->add_option("-p,--pattern", pattern);

    int ret = 0;
    try {
        app.parse(argc, argv);
        return {ParseResult::Continue, ret};
    } catch(const CLI::Success &err) {
        // printing help message or version info
        ret = app.exit(err);
        return {ParseResult::Break, ret};
    } catch(const CLI::ParseError &err) {
        ret = app.exit(err);
        return {ParseResult::Break, ret};
    }
}

void Config::loadPersistentConfig()
{
    const auto persistentConfigFile = outputDir / PERSISTENT_CONFIG;
    if (!fs::is_regular_file(persistentConfigFile)) {
        return;
    }
    try {
        const auto data = toml::parse(persistentConfigFile);
        if (pattern.empty()) {
            pattern = toml::find<std::string>(data, "pattern");
        }
    } catch (const std::exception& err) {
        spdlog::error("Failed to load persistent config: " + std::string{err.what()});
    }
}

void Config::storePersistentConfig() const
{
    if (!fs::is_directory(outputDir)) {
        return;
    }
    const auto persistentConfigFile = outputDir / PERSISTENT_CONFIG;
    toml::value data{{"pattern", pattern}};
    std::ofstream out{persistentConfigFile};
    out << "# this file is updated on every run of mediacopier"
        << ", manual changes might be lost\n" << data;
}

void Config::finalize() noexcept
{
    if (pattern.empty()) {
        pattern = DEFAULT_PATTERN;
    }
}
