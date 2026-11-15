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
#include <mediacopier/file_status.hpp>
#include <mediacopier/operation_copy_jpeg.hpp>

namespace fs = std::filesystem;

namespace mediacopier::test {

const constexpr char* DEFAULT_PATTERN = "%Y/%m/%d/TEST_%Y%m%d_%H%M%S";

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
        ASSERT_EQ(get_file_status(srcPath), FileStatus::Duplicate);

        FileOperationCopyJpeg copy { path1.value() };
        file->accept(copy);

        ASSERT_TRUE(fs::is_regular_file(path1.value()));
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
    ASSERT_EQ(get_file_status(srcPath), FileStatus::Duplicate);
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

TEST_F(FileRegisterTests, removeDuplicateMarksSource)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:14:32");
    img.setExif("SubSecTimeOriginal", "42");
    img.rotate(Rotation::R270);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_270);

    ImageTestFile img2;
    img2.m_path = img.path();
    img2.copy(workdir() / "test2.jpg");

    FileRegister dst { dstdir(), DEFAULT_PATTERN, false };

    auto file1 = to_file_info_ptr(img.path());
    auto path1 = dst.add(file1);
    ASSERT_TRUE(path1.has_value());
    {
        FileOperationCopyJpeg copy { path1.value() };
        file1->accept(copy);
    }

    auto file2 = to_file_info_ptr(img2.path());
    auto path2 = dst.add(file2);
    ASSERT_TRUE(path2.has_value());
    {
        FileOperationCopyJpeg copy { path2.value() };
        file2->accept(copy);
    }

    dst.removeDuplicates();

    ASSERT_FALSE(fs::exists(path2.value()));
    ASSERT_EQ(get_file_status(img2.path()), FileStatus::Duplicate);
}

} // namespace mediacopier::test
