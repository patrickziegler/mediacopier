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

#include <mediacopier/file_status.hpp>

#include <fstream>

namespace fs = std::filesystem;

namespace mediacopier::test {

TEST(FileStatusTests, stringConversion)
{
    ASSERT_EQ(to_string(FileStatus::Copied), "copied");
    ASSERT_EQ(to_string(FileStatus::Duplicate), "duplicate");
    ASSERT_EQ(to_string(FileStatus::Error), "error");
    ASSERT_EQ(to_string(FileStatus::Unsupported), "unsupported");
    ASSERT_EQ(to_string(FileStatus::Unknown), "unknown");

    ASSERT_EQ(from_string("copied"), FileStatus::Copied);
    ASSERT_EQ(from_string("duplicate"), FileStatus::Duplicate);
    ASSERT_EQ(from_string("error"), FileStatus::Error);
    ASSERT_EQ(from_string("unsupported"), FileStatus::Unsupported);
    ASSERT_EQ(from_string("unknown"), FileStatus::Unknown);
    ASSERT_EQ(from_string("invalid"), FileStatus::Unknown);
}

TEST_F(CommonTestFixtures, setAndGetFileStatus)
{
    const auto path = workdir() / "file.txt";
    std::ofstream { path } << "test";

    ASSERT_EQ(get_file_status(path), FileStatus::Unknown);

    for (const auto status : { FileStatus::Copied, FileStatus::Duplicate, FileStatus::Error, FileStatus::Unsupported }) {
        ASSERT_TRUE(set_file_status(path, status));
        ASSERT_EQ(get_file_status(path), status);
    }

    ASSERT_TRUE(clear_file_status(path));
    ASSERT_EQ(get_file_status(path), FileStatus::Unknown);
}

TEST_F(CommonTestFixtures, missingFileHasNoStatus)
{
    const auto path = workdir() / "does-not-exist.txt";

    ASSERT_EQ(get_file_status(path), FileStatus::Unknown);
    ASSERT_FALSE(set_file_status(path, FileStatus::Copied));
}

} // namespace mediacopier::test
