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
public:
    auto dstdir() const
    {
        return workdir() / "dst";
    }
    auto executeCopyOperation(const fs::path& srcPath) const
    {
        fs::remove_all(dstdir());

        auto file = FileInfoFactory::createFromPath(srcPath);

        FileRegister dst { dstdir(), DEFAULT_PATTERN, false };
        auto path1 = dst.add(file);
        ASSERT_TRUE(path1.has_value());

        // both srcPath are binary equal, file should be ignored
        auto path2 = dst.add(file);
        ASSERT_FALSE(path2.has_value());

        FileOperationCopyJpeg copy { path1.value() };
        file->accept(copy);
    }
};

TEST_F(FileRegisterTests, multipleAddSameSource)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:10:32");
    img.setExif("SubSecTimeOriginal", "123456");
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);

    const auto srcPath = img.path();

    executeCopyOperation(srcPath);

    FileRegister dst { dstdir(), DEFAULT_PATTERN, false };

    // binary equal destination already exists, file should be ignored
    auto path = dst.add(to_file_info_ptr(srcPath));
    ASSERT_FALSE(path.has_value());
}

TEST_F(FileRegisterTests, multipleAddDifferentSource)
{
    ImageTestFile img1;
    img1.convert(workdir() / "test1.jpg");
    img1.rotate(Rotation::R270);
    img1.setExif("DateTimeOriginal", "2019-02-05 12:13:32");
    img1.setExif("SubSecTimeOriginal", "123");
    img1.setExif(FileInfoImageJpeg::Orientation::ROT_270);

    ImageTestFile img2;
    img2.convert(workdir() / "test2.jpg", "64x64", 20);
    img2.rotate(Rotation::R270);
    img2.setExif("DateTimeOriginal", "2019-02-05 12:13:32");
    img2.setExif("SubSecTimeOriginal", "123");
    img2.setExif(FileInfoImageJpeg::Orientation::ROT_270);

    const auto srcPath = img1.path();
    const auto srcPathMod = img2.path();

    executeCopyOperation(srcPath);

    FileRegister dst { dstdir(), DEFAULT_PATTERN, false };

    // file is added because image at destination is different
    auto path1 = dst.add(to_file_info_ptr(srcPath));
    ASSERT_TRUE(path1.has_value());

    // file is added because image at source is different
    auto path2 = dst.add(to_file_info_ptr(srcPathMod));
    ASSERT_TRUE(path2.has_value());
}

} // namespace mediacopier::test
