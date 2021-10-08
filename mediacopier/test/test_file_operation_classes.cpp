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

#include <mediacopier/file_operation_move.hpp>
#include <mediacopier/file_operation_move_jpeg.hpp>
#include <mediacopier/file_register.hpp>

#include "common_test_fixtures.hpp"

namespace fs = std::filesystem;

namespace mediacopier::test {

template <typename T>
static auto execute_operation(const fs::path& srcPath, const fs::path& dstBaseDir) -> const fs::path
{
    FileRegister dst{dstBaseDir, DEFAULT_PATTERN};
    dst.add(srcPath);
    const auto& it = dst.begin();
    const auto& dest = it->first;
    const auto& file = it->second;
    T op{dest};
    file->accept(op);
    return dest;
}

class FileOperationTests : public CommonTestFixtures {
protected:
    auto checkAllOperations(std::string srcName, std::string dstName, std::string timestamp, const FileInfoImageJpeg::Orientation& orientation, const FileInfoImageJpeg::Orientation& orientationFixed, std::function<void (fs::path, const FileInfoImageJpeg::Orientation&, const std::string&)> checkFileInfoCustom) -> void
    {
        fs::remove_all(m_dstBaseDir1);
        fs::remove_all(m_dstBaseDir2);

        fs::path srcPath, dstPath;

        // copy src -> dst1
        srcPath = m_testDataDirOrig / srcName;
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
        srcPath = m_testDataDirOrig / srcName;
        dstPath = execute_operation<FileOperationCopyJpeg>(srcPath, m_dstBaseDir2);
        checkFileInfoCustom(dstPath, orientationFixed, timestamp);
        ASSERT_TRUE(fs::exists(srcPath));
    }

protected:
    using CommonTestFixtures::SetUp;
    fs::path m_dstBaseDir1 = m_testDataDir / "tmp1";
    fs::path m_dstBaseDir2 = m_testDataDir / "tmp2";
};

TEST_F(FileOperationTests, singleImageJpeg0ImageAllOperations)
{
    std::string srcName = "lena64_rot0.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121032_123456.jpg";
    std::string timestamp = "2019-02-05 12:10:32.123456";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_0;
    const auto& orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(std::move(dstPath), orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleImageJpeg90ImageAllOperations)
{
    std::string srcName = "lena64_rot90.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121132_000000.jpg";
    std::string timestamp = "2019-02-05 12:11:32";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_90;
    const auto& orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(std::move(dstPath), orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleImageJpeg180ImageAllOperations)
{
    std::string srcName = "lena64_rot180.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121232_123400.jpg";
    std::string timestamp = "2019-02-05 12:12:32.1234";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_180;
    const auto& orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(std::move(dstPath), orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleImageJpeg270ImageAllOperations)
{
    std::string srcName = "lena64_rot270.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121332_123000.jpg";
    std::string timestamp = "2019-02-05 12:13:32.123";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_270;
    const auto& orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(std::move(dstPath), orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleImageJpeg90WrongSizeImageAllOperations)
{
    std::string srcName = "lena50_rot90.jpg";
    std::string dstName = "2018/05/05/TEST_20180505_061132_000000.jpg";
    std::string timestamp = "2018-05-05 06:11:32";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_90;
    const auto& orientationFixed = orientation;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpegAttrs(std::move(dstPath), orientation, timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleImageAllOperations)
{
    std::string srcName = "lena16_rot90.tiff";
    std::string dstName = "2019/02/05/TEST_20190205_120932_000000.tiff";
    std::string timestamp = "2019-02-05 12:09:32";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_0;
    const auto& orientationFixed = FileInfoImageJpeg::Orientation::ROT_0;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoAttrs(std::move(dstPath), timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

TEST_F(FileOperationTests, singleVideoAllOperations)
{
    std::string srcName = "roundhay_garden_scene.mp4";
    std::string dstName = "2018/01/01/TEST_20180101_010101_000000.mp4";
    std::string timestamp = "2018-01-01 01:01:01";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_0;
    const auto& orientationFixed = orientation;

    auto checkFileInfoCustom = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoAttrs(std::move(dstPath), timestamp);
    };

    checkAllOperations(srcName, dstName, timestamp, orientation, orientationFixed, checkFileInfoCustom);
}

} // namespace mediacopier::test
