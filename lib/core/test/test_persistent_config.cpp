/* Copyright (C) 2026 Patrick Ziegler <zipat@proton.me>
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

#include "common_test_fixtures.hpp"

#include <mediacopier/persistent_config.hpp>

namespace fs = std::filesystem;

namespace mediacopier::test {

class TestConfig : public PersistentConfig {
public:
    auto& pattern()
    {
        return m_pattern;
    }
    auto& useUtc()
    {
        return m_useUtc;
    }
};

class PersistentConfigTests : public CommonTestFixtures {
};

TEST_F(PersistentConfigTests, test1)
{
    fs::path testdir = workdir() / generate_random_hash();
    fs::create_directory(testdir);

    TestConfig cfg1;
    cfg1.pattern().set("TEST_%Y%m%d_%H%M%S");
    cfg1.useUtc().set(false);
    cfg1.storePersistentConfig(testdir);

    TestConfig cfg2;
    cfg2.loadPersistentConfig(testdir);

    ASSERT_TRUE(cfg1.pattern().get() == cfg2.pattern().get());
    ASSERT_TRUE(cfg1.useUtc().get() == cfg2.useUtc().get());

    ASSERT_TRUE(cfg2.pattern().get() == "TEST_%Y%m%d_%H%M%S");
    ASSERT_TRUE(cfg2.useUtc().get() == false);
}

} // namespace mediacopier::test
