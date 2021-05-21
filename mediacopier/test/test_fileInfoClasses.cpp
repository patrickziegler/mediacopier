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

namespace fs = std::filesystem;

namespace MediaCopier::Test {

class FileInfoTests : public CommonTestFixtures {
    using CommonTestFixtures::SetUp;
};

TEST_F(FileInfoTests, validFileInfoImageJpeg)
{
    checkFileInfoType(test_data_dir_orig / "lena64_rot0.jpg",
                      FileInfoType::FileInfoImageJpeg);

    checkFileInfoJpeg(test_data_dir_orig / "lena64_rot0.jpg",
                      FileInfoImageJpeg::Orientation::ROT_0, "2019-02-05 12:10:32.123456");

    checkFileInfoJpeg(test_data_dir_orig / "lena64_rot90.jpg",
                      FileInfoImageJpeg::Orientation::ROT_90, "2019-02-05 12:11:32");

    checkFileInfoJpeg(test_data_dir_orig / "lena64_rot180.jpg",
                      FileInfoImageJpeg::Orientation::ROT_180, "2019-02-05 12:12:32.1234");

    checkFileInfoJpeg(test_data_dir_orig / "lena64_rot270.jpg",
                      FileInfoImageJpeg::Orientation::ROT_270, "2019-02-05 12:13:32.123");
}

TEST_F(FileInfoTests, invalidFileInfoImageJpeg)
{
    checkFileInfoType(test_data_dir_orig / "lena64_rot270_orientation_missing.jpg",
                      FileInfoType::FileInfoImage);

    checkNullptr(test_data_dir_orig / "lena64_rot270_timestamp_missing.jpg");
}

TEST_F(FileInfoTests, validFileInfoVideo)
{
    checkFileInfoType(test_data_dir_orig / "roundhay_garden_scene.mov",
                      FileInfoType::FileInfoVideo);

    checkFileInfoType(test_data_dir_orig / "roundhay_garden_scene.mp4",
                      FileInfoType::FileInfoVideo);

    checkFileInfoType(test_data_dir_orig / "roundhay_garden_scene.webm",
                      FileInfoType::FileInfoVideo);

    checkFileInfo(test_data_dir_orig / "roundhay_garden_scene.mov", "2018-01-01 01:01:01");
    checkFileInfo(test_data_dir_orig / "roundhay_garden_scene.mp4", "2018-01-01 01:01:01");
    checkFileInfo(test_data_dir_orig / "roundhay_garden_scene.webm", "2018-01-01 01:01:01");
}

TEST_F(FileInfoTests, invalidFileInfoVideo)
{
    checkNullptr(test_data_dir_orig / "video_timestamp_missing.mov");
    checkNullptr(test_data_dir_orig / "video_broken.webm");
}

} // namespace MediaCopier::Test
