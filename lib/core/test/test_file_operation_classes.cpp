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

#include <mediacopier/duplicate_check.hpp>
#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_register.hpp>
#include <mediacopier/operation_copy_jpeg.hpp>
#include <mediacopier/operation_move_jpeg.hpp>
#include <mediacopier/operation_simulate.hpp>

namespace mediacopier::test {

const constexpr char* DEFAULT_PATTERN = "TEST_%Y%m%d_%H%M%S";

template <typename T>
static auto execute_operation(const fs::path& srcPath, const fs::path& dstBaseDir) -> const fs::path
{
    FileRegister destinationRegister { dstBaseDir, DEFAULT_PATTERN, false };
    auto file = FileInfoFactory::createFromPath(srcPath);
    if (file == nullptr) {
        throw std::runtime_error("file not found: " + srcPath.string());
    }
    const auto path = destinationRegister.add(file).value();
    T operation { path };
    file->accept(operation);
    return path;
}

class FileOperationTests : public CommonTestFixtures {
public:
    FileOperationTests()
    {
        m_dstBaseDir1 = workdir() / "tmp1";
        m_dstBaseDir2 = workdir() / "tmp2";
    }
    auto checkAllOperations(const std::string& srcName, const std::string& dstName, const std::string& timestamp,
        const FileInfoImageJpeg::Orientation& orientation, const FileInfoImageJpeg::Orientation& orientationFixed,
        std::function<void(const fs::path&, const FileInfoImageJpeg::Orientation&, const std::string&)> checkFileInfoCustom) const
    {
        fs::remove_all(m_dstBaseDir1);
        fs::remove_all(m_dstBaseDir2);

        fs::path srcPath, dstPath;

        // simulate
        srcPath = srcName;
        ASSERT_NO_THROW(execute_operation<FileOperationSimulate>(srcPath, m_dstBaseDir1));

        // copy src -> dst1
        srcPath = srcName;
        dstPath = execute_operation<FileOperationCopy>(srcPath, m_dstBaseDir1);
        checkFileInfoCustom(dstPath, orientation, timestamp);
        ASSERT_TRUE(fs::exists(srcPath));

        // move dst1 -> dst2
        srcPath = m_dstBaseDir1 / dstName;
        dstPath = execute_operation<FileOperationMove>(srcPath, m_dstBaseDir2);
        checkFileInfoCustom(dstPath, orientation, timestamp);
        ASSERT_FALSE(fs::exists(srcPath));

        // move dst2 -> dst1, jpeg aware
        srcPath = m_dstBaseDir2 / dstName;
        dstPath = execute_operation<FileOperationMoveJpeg>(srcPath, m_dstBaseDir1);
        checkFileInfoCustom(dstPath, orientationFixed, timestamp);
        ASSERT_FALSE(fs::exists(srcPath));

        // copy src -> dst2, jpeg aware
        srcPath = srcName;
        dstPath = execute_operation<FileOperationCopyJpeg>(srcPath, m_dstBaseDir2);
        checkFileInfoCustom(dstPath, orientationFixed, timestamp);
        ASSERT_TRUE(fs::exists(srcPath));
    }
    const auto& dstdir()
    {
        return m_dstBaseDir2;
    }

private:
    fs::path m_dstBaseDir1;
    fs::path m_dstBaseDir2;
};

TEST_F(FileOperationTests, singleImageJpeg0ImageAllOperations)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:10:32");
    img.setExif("SubSecTimeOriginal", "123456");
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);

    const auto srcName = img.path();
    const std::string dstName = "TEST_20190205_121032.123456000.jpg";
    const std::string timestamp = "2019-02-05 12:10:32.123456";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_0;
    const auto orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(dstPath, orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleImageJpeg90ImageAllOperations)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:11:32");
    img.rotate(Rotation::R90);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_90);

    const auto srcName = img.path();
    const std::string dstName = "TEST_20190205_121132.000000000.jpg";
    const std::string timestamp = "2019-02-05 12:11:32";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_90;
    const auto orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(dstPath, orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);

    img.rotate(Rotation::R270, true, false);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);

    ASSERT_TRUE(is_duplicate(img.path(), dstdir() / dstName));
}

TEST_F(FileOperationTests, singleImageJpeg180ImageAllOperations)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:12:32");
    img.setExif("SubSecTimeOriginal", "1234");
    img.rotate(Rotation::R180);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_180);

    const auto srcName = img.path();
    const std::string dstName = "TEST_20190205_121232.123400000.jpg";
    const std::string timestamp = "2019-02-05 12:12:32.1234";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_180;
    const auto orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(dstPath, orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);

    img.rotate(Rotation::R180, true, false);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);

    ASSERT_TRUE(is_duplicate(img.path(), dstdir() / dstName));
}

TEST_F(FileOperationTests, singleImageJpeg270ImageAllOperations)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:13:32");
    img.setExif("SubSecTimeOriginal", "123");
    img.rotate(Rotation::R270);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_270);

    const auto srcName = img.path();
    const std::string dstName = "TEST_20190205_121332.123000000.jpg";
    const std::string timestamp = "2019-02-05 12:13:32.123";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_270;
    const auto orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(dstPath, orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);

    img.rotate(Rotation::R90, true, false);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);

    ASSERT_TRUE(is_duplicate(img.path(), dstdir() / dstName));
}

TEST_F(FileOperationTests, singleImageJpeg90WrongSizeImageAllOperations)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg", "50x50");
    img.setExif("DateTimeOriginal", "2018-05-05 06:11:32");
    img.rotate(Rotation::R90, false);
    img.setExif(FileInfoImageJpeg::Orientation::ROT_90);

    const auto srcName = img.path();
    const std::string dstName = "TEST_20180505_061132.000000000.jpg";
    const std::string timestamp = "2018-05-05 06:11:32";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_90;
    const auto orientationFixed = orientation;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(dstPath, orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);

    ASSERT_TRUE(is_duplicate(img.path(), dstdir() / dstName));
}

TEST_F(FileOperationTests, singleRawImageAllOperations)
{
    ImageTestFile img;
    img.copy(workdir() / "test.tiff");
    img.setExif("DateTimeOriginal", "2019-02-05 12:09:32");
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);

    checkFileInfoType(img.path(), FileInfoType::FileInfoImage);

    const auto srcName = img.path();
    const std::string dstName = "TEST_20190205_120932.000000000.tiff";
    const std::string timestamp = "2019-02-05 12:09:32";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_0;
    const auto orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoDateTime(dstPath, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);

    ASSERT_TRUE(is_duplicate(img.path(), dstdir() / dstName));
}

TEST_F(FileOperationTests, singleVideoAllOperations)
{
    VideoTestFile vid;
    vid.copy(workdir() / "test.mp4");
    vid.setCreationTime("2018-01-01 01:01:01Z");

    const auto srcName = vid.path();
    const std::string dstName = "TEST_20180101_010101.000000000.mp4";
    const std::string timestamp = "2018-01-01 01:01:01";
    const auto orientation = FileInfoImageJpeg::Orientation::ROT_0;
    const auto orientationFixed = orientation;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoDateTime(dstPath, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);

    ASSERT_TRUE(is_duplicate(vid.path(), dstdir() / dstName));
}

} // namespace mediacopier::test
