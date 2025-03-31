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

#include "common_test_fixtures.hpp"

#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_register.hpp>
#include <mediacopier/operation_copy_jpeg.hpp>

namespace fs = std::filesystem;

namespace mediacopier::test {

class FileRegisterTests : public CommonTestFixtures {
protected:
    using CommonTestFixtures::SetUp;

    auto executeCopyOperation(const fs::path& srcPath) -> void
    {
        fs::remove_all(m_dstBaseDir);

        auto file = FileInfoFactory::createFromPath(srcPath);

        FileRegister dst{m_dstBaseDir, DEFAULT_PATTERN, false};
        auto path1 = dst.add(file);
        ASSERT_TRUE(path1.has_value());

        // both srcPath are binary equal, file should be ignored
        auto path2 = dst.add(file);
        ASSERT_FALSE(path2.has_value());

        FileOperationCopyJpeg copy{path1.value()};
        file->accept(copy);
    }

    fs::path m_dstBaseDir = m_testDataDir / "tmp";
};

TEST_F(FileRegisterTests, multipleAddSameSource)
{
    const auto& srcPath = m_testDataDirOrig / "lena64_rot0.jpg";

    executeCopyOperation(srcPath);

    FileRegister dst{m_dstBaseDir, DEFAULT_PATTERN, false};

    // binary equal destination already exists, file should be ignored
    auto path = dst.add(to_file_info_ptr(srcPath));
    ASSERT_FALSE(path.has_value());
}

TEST_F(FileRegisterTests, multipleAddDifferentSource)
{
    const auto& srcPath = m_testDataDirOrig / "lena64_rot270.jpg";
    const auto& srcPathMod = m_testDataDirOrig / "lena64_rot270_low_quality.jpg";

    executeCopyOperation(srcPath);

    FileRegister dst{m_dstBaseDir, DEFAULT_PATTERN, false};

    // file is added because image at destination is different
    auto path1 = dst.add(to_file_info_ptr(srcPath));
    ASSERT_TRUE(path1.has_value());

    // file is added because image at source is different
    auto path2 = dst.add(to_file_info_ptr(srcPathMod));
    ASSERT_TRUE(path2.has_value());
}

} // namespace mediacopier::test
