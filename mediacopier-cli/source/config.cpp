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

#include <mediacopier/version.hpp>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

constexpr static const char* PERSISTENT_CONFIG = ".mediacopier";

std::pair<Config::ParseResult, int> Config::parseArgs(int argc, char *argv[])
{
    CLI::App app{mediacopier::MEDIACOPIER_PROJECT_NAME};
    app.set_version_flag("-v,--version", mediacopier::MEDIACOPIER_VERSION);

    static const auto& isValidPath = [](const std::string& path) -> std::string {
        try {
            if (fs::absolute(path).empty()) {
                throw CLI::ValidationError{"Empty string is not a valid path"};
            }
        } catch(const fs::filesystem_error& err) {
            throw CLI::ValidationError{"Invalid path definition: " + std::string{err.what()}};
        }
        return {};
    };

    const auto& setUseUtc = [this](size_t /* count */) -> void { m_useUtc = true; };

    auto copyapp = app.add_subcommand("copy", "Copy some files");
    copyapp->callback([this]() { m_command = Command::Copy; });
    copyapp->add_option("inputDir", m_inputDir)->required()->check(CLI::ExistingDirectory);
    copyapp->add_option("outputDir", m_outputDir)->required()->check(isValidPath, "DIR");
    copyapp->add_option("-p,--pattern", m_pattern, "Pattern to be used for constructing filenames");
    copyapp->add_flag("-u,--utc", setUseUtc, "Use UTC timestamps when constructing filenames");

    auto moveapp = app.add_subcommand("move", "Move some files");
    moveapp->callback([this]() { m_command = Command::Move; });
    moveapp->add_option("inputDir", m_inputDir)->required()->check(CLI::ExistingDirectory);
    moveapp->add_option("outputDir", m_outputDir)->required()->check(isValidPath, "DIR");
    moveapp->add_option("-p,--pattern", m_pattern, "Pattern to be used for constructing filenames");
    moveapp->add_flag("-u,--utc", setUseUtc, "Use UTC timestamps when constructing filenames");

    auto simapp = app.add_subcommand("sim", "Simulate operation and dump info");
    simapp->callback([this]() { m_command = Command::Sim; });
    simapp->add_option("inputDir", m_inputDir)->required()->check(CLI::ExistingDirectory);
    simapp->add_option("outputDir", m_outputDir)->required()->check(isValidPath, "DIR");
    simapp->add_option("-p,--pattern", m_pattern, "Pattern to be used for constructing filenames");
    simapp->add_flag("-u,--utc", setUseUtc, "Use UTC timestamps when constructing filenames");

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
    const auto persistentConfigFile = m_outputDir / PERSISTENT_CONFIG;
    if (!fs::is_regular_file(persistentConfigFile)) {
        return;
    }
    const toml::value input = toml::parse(persistentConfigFile);
    if(input.contains("pattern") && input.at("pattern").is_string()) {
        m_pattern.setDefault(input.at("pattern").as_string());
    }
    if(input.contains("useUtc") && input.at("useUtc").is_boolean()) {
        m_useUtc.setDefault(input.at("useUtc").as_boolean());
    }
}

void Config::storePersistentConfig() const
{
    if (!fs::is_directory(m_outputDir)) {
        return;
    }
    const auto persistentConfigFile = m_outputDir / PERSISTENT_CONFIG;
    toml::value output{{"pattern", m_pattern.get()}, {"useUtc", m_useUtc.get()}};
    std::ofstream os{persistentConfigFile};
    os << "# this file is updated on every run of mediacopier"
       << ", manual changes might be lost\n" << output;
}
