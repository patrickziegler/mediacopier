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

#include "common_test_fixtures.hpp"

namespace fs = std::filesystem;

namespace mediacopier::test {

class FileInfoTests : public CommonTestFixtures {
    using CommonTestFixtures::SetUp;
};

TEST_F(FileInfoTests, validFileInfoImageJpeg)
{
    checkFileInfoType(m_testDataDirOrig / "lena64_rot0.jpg", FileInfoType::FileInfoImageJpeg);
    checkFileInfoJpegAttrs(m_testDataDirOrig / "lena64_rot0.jpg", FileInfoImageJpeg::Orientation::ROT_0, "2019-02-05 12:10:32.123456");
    checkFileInfoJpegAttrs(m_testDataDirOrig / "lena64_rot90.jpg", FileInfoImageJpeg::Orientation::ROT_90, "2019-02-05 12:11:32");
    checkFileInfoJpegAttrs(m_testDataDirOrig / "lena64_rot180.jpg", FileInfoImageJpeg::Orientation::ROT_180, "2019-02-05 12:12:32.1234");
    checkFileInfoJpegAttrs(m_testDataDirOrig / "lena64_rot270.jpg", FileInfoImageJpeg::Orientation::ROT_270, "2019-02-05 12:13:32.123");
}

TEST_F(FileInfoTests, invalidFileInfoImageJpeg)
{
    checkFileInfoType(m_testDataDirOrig / "lena64_rot270_orientation_missing.jpg", FileInfoType::FileInfoImage);
    checkFileValid(m_testDataDirOrig / "lena64_rot270_timestamp_missing.jpg");
}

TEST_F(FileInfoTests, validFileInfoVideo)
{
    checkFileInfoType(m_testDataDirOrig / "roundhay_garden_scene.mov", FileInfoType::FileInfoVideo);
    checkFileInfoType(m_testDataDirOrig / "roundhay_garden_scene.mp4", FileInfoType::FileInfoVideo);
    checkFileInfoType(m_testDataDirOrig / "roundhay_garden_scene.webm", FileInfoType::FileInfoVideo);
    checkFileInfoAttrs(m_testDataDirOrig / "roundhay_garden_scene.mov", "2018-01-01 01:01:01");
    checkFileInfoAttrs(m_testDataDirOrig / "roundhay_garden_scene.mp4", "2018-01-01 01:01:01");
    checkFileInfoAttrs(m_testDataDirOrig / "roundhay_garden_scene.webm", "2018-01-01 01:01:01");
}

TEST_F(FileInfoTests, invalidFileInfoVideo)
{
    checkFileValid(m_testDataDirOrig / "video_timestamp_missing.mov");
    checkFileValid(m_testDataDirOrig / "video_broken.webm");
}

TEST_F(FileInfoTests, unkownFileType)
{
    checkFileValid(m_testDataDirOrig / "dummy.txt");
}

} // namespace mediacopier::test
