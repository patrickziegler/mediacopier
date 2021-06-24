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
#include <mediacopier/FileOperationMove.hpp>
#include <mediacopier/FileOperationMoveJpeg.hpp>

namespace fs = std::filesystem;

namespace MediaCopier::Test {

template <typename T>
static const fs::path execOnSingleFile(const MediaCopier::FileRegister& reg)
{
    const auto& it = reg.begin();
    const auto& dest = it->first;
    const auto& file = it->second;
    T op{dest};
    file->accept(op);
    return {dest};
}

class FileOperationTests : public CommonTestFixtures {
protected:
    void checkOps(std::string srcName, std::string dstName, std::string timestamp, const FileInfoImageJpeg::Orientation& orientation1, const FileInfoImageJpeg::Orientation& orientation2, std::function<void (fs::path, const FileInfoImageJpeg::Orientation&, const std::string&)> check)
    {
        fs::path srcPath, dstPath;

        fs::remove_all(test_data_dir_dst1);
        fs::remove_all(test_data_dir_dst2);

        FileRegister dst1{test_data_dir_dst1, "%Y/%m/%d/TEST_%Y%m%d_%H%M%S_"};
        FileRegister dst2{test_data_dir_dst2, "%Y/%m/%d/TEST_%Y%m%d_%H%M%S_"};

        // copy src -> dst1
        srcPath = test_data_dir_orig / srcName;
        dst1.reset();
        dst1.add(srcPath);
        dstPath = execOnSingleFile<FileOperationCopy>(dst1);
        check(dstPath, orientation1, timestamp);
        ASSERT_TRUE(fs::exists(srcPath));

        // move dst1 -> dst2
        srcPath = test_data_dir_dst1 / dstName;
        dst2.reset();
        dst2.add(srcPath);
        dstPath = execOnSingleFile<FileOperationMove>(dst2);
        check(dstPath, orientation1, timestamp);
        ASSERT_FALSE(fs::exists(srcPath));

        // move dst2 -> dst1, jpeg aware
        srcPath = test_data_dir_dst2 / dstName;
        dst1.reset();
        dst1.add(srcPath);
        dstPath = execOnSingleFile<FileOperationMoveJpeg>(dst1);
        check(dstPath, orientation2, timestamp);
        ASSERT_FALSE(fs::exists(srcPath));

        // copy src -> dst2, jpeg aware
        srcPath = test_data_dir_orig / srcName;
        dst2.reset();
        dst2.add(srcPath);
        dstPath = execOnSingleFile<FileOperationCopyJpeg>(dst2);
        check(dstPath, orientation2, timestamp);
        ASSERT_TRUE(fs::exists(srcPath));
    }

private:
    using CommonTestFixtures::SetUp;
    fs::path test_data_dir_dst1 = test_data_dir / "tmp1";
    fs::path test_data_dir_dst2 = test_data_dir / "tmp2";
};

TEST_F(FileOperationTests, singleImageJpeg0ImageAllOperations)
{
    std::string srcName = "lena64_rot0.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121032_123456.jpg";
    std::string timestamp = "2019-02-05 12:10:32.123456";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_0;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpeg(std::move(dstPath), orientation, timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_0, check);
}

TEST_F(FileOperationTests, singleImageJpeg90ImageAllOperations)
{
    std::string srcName = "lena64_rot90.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121132_000000.jpg";
    std::string timestamp = "2019-02-05 12:11:32";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_90;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpeg(std::move(dstPath), orientation, timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_0, check);
}

TEST_F(FileOperationTests, singleImageJpeg180ImageAllOperations)
{
    std::string srcName = "lena64_rot180.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121232_123400.jpg";
    std::string timestamp = "2019-02-05 12:12:32.1234";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_180;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpeg(std::move(dstPath), orientation, timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_0, check);
}

TEST_F(FileOperationTests, singleImageJpeg270ImageAllOperations)
{
    std::string srcName = "lena64_rot270.jpg";
    std::string dstName = "2019/02/05/TEST_20190205_121332_123000.jpg";
    std::string timestamp = "2019-02-05 12:13:32.123";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_270;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpeg(std::move(dstPath), orientation, timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_0, check);
}

TEST_F(FileOperationTests, singleImageJpeg90WrongSizeImageAllOperations)
{
    std::string srcName = "lena50_rot90.jpg";
    std::string dstName = "2018/05/05/TEST_20180505_061132_000000.jpg";
    std::string timestamp = "2018-05-05 06:11:32";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_90;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfoJpeg(std::move(dstPath), orientation, timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_90, check);
}

TEST_F(FileOperationTests, singleImageAllOperations)
{
    std::string srcName = "lena16_rot90.tiff";
    std::string dstName = "2019/02/05/TEST_20190205_120932_000000.tiff";
    std::string timestamp = "2019-02-05 12:09:32";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_0;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfo(std::move(dstPath), timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_0, check);
}

TEST_F(FileOperationTests, singleVideoAllOperations)
{
    std::string srcName = "roundhay_garden_scene.mp4";
    std::string dstName = "2018/01/01/TEST_20180101_010101_000000.mp4";
    std::string timestamp = "2018-01-01 01:01:01";

    const auto& orientation = FileInfoImageJpeg::Orientation::ROT_0;

    auto check = [this](fs::path dstPath, const FileInfoImageJpeg::Orientation& orientation, const std::string& timestamp) -> void {
        checkFileInfo(std::move(dstPath), timestamp);
    };

    checkOps(srcName, dstName, timestamp, orientation, FileInfoImageJpeg::Orientation::ROT_0, check);
}

} // namespace MediaCopier::Test
