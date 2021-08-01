/* Copyright (C) 2021 Patrick Ziegler
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

#include "CommonTestFixtures.hpp"

#include <mediacopier/FileRegister.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>

namespace fs = std::filesystem;

namespace MediaCopier::Test {

class FileRegisterTests : public CommonTestFixtures {
protected:
    using CommonTestFixtures::SetUp;

    void executeCopyOperation(const fs::path& srcPath)
    {
        fs::remove_all(m_dstBaseDir);

        FileRegister dst{m_dstBaseDir, DEFAULT_PATTERN};
        dst.add(srcPath);

        // both srcPath are binary equal, file should be ignored
        dst.add(srcPath);
        ASSERT_EQ(dst.size(), 1);

        for (const auto& [destination, file] : dst) {
            FileOperationCopyJpeg op{destination};
            file->accept(op);
        }

        // reset works as inteneded
        dst.reset();
        ASSERT_EQ(dst.size(), 0);
    }

    fs::path m_dstBaseDir = m_testDataDir / "tmp";
};

TEST_F(FileRegisterTests, multipleAddSameSource)
{
    const auto& srcPath = m_testDataDirOrig / "lena64_rot0.jpg";

    executeCopyOperation(srcPath);

    FileRegister dst{m_dstBaseDir, DEFAULT_PATTERN};

    // binary equal destination already exists, file should be ignored
    dst.add(srcPath);
    ASSERT_EQ(dst.size(), 0);
}

TEST_F(FileRegisterTests, multipleAddDifferentSource)
{
    const auto& srcPath = m_testDataDirOrig / "lena64_rot270.jpg";
    const auto& srcPathMod = m_testDataDirOrig / "lena64_rot270_orientation_missing.jpg";

    executeCopyOperation(srcPath);

    FileRegister dst{m_dstBaseDir, DEFAULT_PATTERN};

    // file is added because destination is different
    dst.add(srcPath);
    ASSERT_EQ(dst.size(), 1);

    // file is added because source is different
    dst.add(srcPathMod);
    ASSERT_EQ(dst.size(), 2);
}

} // namespace MediaCopier::Test
