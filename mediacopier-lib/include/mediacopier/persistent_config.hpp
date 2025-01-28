/* Copyright (C) 2025 Patrick Ziegler <zipat@proton.me>
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

#pragma once

#include <filesystem>
#include <optional>

template <typename T>
class Configurable {
public:
    Configurable(const T& defaultValue) :
        originalValue{defaultValue},
        defaultValue{defaultValue},
        currentValue{std::nullopt} {}
    Configurable& operator=(const T& value) {
        currentValue = value;
        return *this;
    }
    operator T() const {
        return currentValue.value_or(defaultValue);
    }
    void set(const T& value) {
        currentValue = value;
    }
    void setDefault(const T& value) {
        defaultValue = value;
    }
    T get() const {
        return currentValue.value_or(defaultValue);
    }
    void reset() {
        currentValue.reset();
    }
    void resetDefault() {
        defaultValue = originalValue;
    }
private:
    T originalValue;
    T defaultValue;
    std::optional<T> currentValue;
};

namespace mediacopier {

constexpr const char* DEFAULT_PATTERN = "%Y/%W/IMG_%Y%m%d_%H%M%S";
constexpr const bool DEFAULT_USE_UTC = true;

class PersistentConfig {
public:
    virtual ~PersistentConfig() = default;
    auto loadPersistentConfig(const std::filesystem::path& path) -> void;
    auto storePersistentConfig(const std::filesystem::path& path) const -> void;
protected:
    Configurable<std::string> m_pattern{DEFAULT_PATTERN};
    Configurable<bool> m_useUtc{DEFAULT_USE_UTC};
};

} // namespace mediacopier
