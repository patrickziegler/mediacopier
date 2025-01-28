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

#include <mediacopier/persistent_config.hpp>

#include <toml.hpp>

constexpr static const char* PERSISTENT_CONFIG = ".mediacopier";

namespace fs = std::filesystem;

namespace mediacopier {

auto PersistentConfig::loadPersistentConfig(const fs::path& outputDir) -> void
{
    const auto persistentConfigFile = outputDir / PERSISTENT_CONFIG;
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

auto PersistentConfig::storePersistentConfig(const fs::path& outputDir) const -> void
{
    if (!fs::is_directory(outputDir)) {
        return;
    }
    const auto persistentConfigFile = outputDir / PERSISTENT_CONFIG;
    toml::value output{{"pattern", m_pattern.get()}, {"useUtc", m_useUtc.get()}};
    std::ofstream os{persistentConfigFile};
    os << "# this file is updated on every run of mediacopier"
       << ", manual changes might be lost\n" << output;
}

} // namespace mediacopier
