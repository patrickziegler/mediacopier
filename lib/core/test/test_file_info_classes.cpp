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
#include <fstream>

namespace mediacopier::test {

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class FileInfoTests : public CommonTestFixtures {
};

TEST_F(FileInfoTests, validFileInfoImageJpegOrientation)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:10:32");
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);
    checkFileInfoType(img.path(), FileInfoType::FileInfoImageJpeg);

    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);
    checkFileInfoJpegOrientation(img.path(), FileInfoImageJpeg::Orientation::ROT_0);

    img.setExif(FileInfoImageJpeg::Orientation::ROT_90);
    checkFileInfoJpegOrientation(img.path(), FileInfoImageJpeg::Orientation::ROT_90);

    img.setExif(FileInfoImageJpeg::Orientation::ROT_180);
    checkFileInfoJpegOrientation(img.path(), FileInfoImageJpeg::Orientation::ROT_180);

    img.setExif(FileInfoImageJpeg::Orientation::ROT_270);
    checkFileInfoJpegOrientation(img.path(), FileInfoImageJpeg::Orientation::ROT_270);
}

TEST_F(FileInfoTests, validFileInfoImageJpegDateTime)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:10:32");
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);
    checkFileInfoType(img.path(), FileInfoType::FileInfoImageJpeg);
    checkFileInfoDateTime(img.path(), "2019-02-05 12:10:32");

    img.setExif("SubSecTimeOriginal", "123456");
    checkFileInfoDateTime(img.path(), "2019-02-05 12:10:32.123456");

    img.setExif("SubSecTimeOriginal", "32");
    checkFileInfoDateTime(img.path(), "2019-02-05 12:10:32.32");

    img.setExif("SubSecTimeOriginal", "1234");
    checkFileInfoDateTime(img.path(), "2019-02-05 12:10:32.1234");

    img.setExif("SubSecTimeOriginal", "123");
    checkFileInfoDateTime(img.path(), "2019-02-05 12:10:32.123");
}

TEST_F(FileInfoTests, validFileInfoImageRaw)
{
    ImageTestFile img;
    img.copy(workdir() / "test.tiff");
    img.setExif("DateTimeOriginal", "2019-02-05 12:10:32");
    img.setExif("SubSecTimeOriginal", "123456");
    img.setExif(FileInfoImageJpeg::Orientation::ROT_0);
    checkFileInfoType(img.path(), FileInfoType::FileInfoImage);
    checkFileInfoDateTime(img.path(), "2019-02-05 12:10:32.123456");
}

TEST_F(FileInfoTests, invalidFileInfoImageOrientationMissing)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    img.setExif("DateTimeOriginal", "2019-02-05 12:10:32");
    // Orientation missing, should be FileInfoImage type
    checkFileInfoType(img.path(), FileInfoType::FileInfoImage);
}

TEST_F(FileInfoTests, invalidFileInfoImageDateTimeMissing)
{
    ImageTestFile img;
    img.convert(workdir() / "test.jpg");
    // DateTime missing, should be invalid
    checkFileInvalid(img.path());
}

TEST_F(FileInfoTests, validFileInfoVideo)
{
    VideoTestFile vid1;
    vid1.copy(workdir() / "test1.mp4");
    vid1.setCreationTime("2018-01-01 01:01:01Z");
    checkFileInfoType(vid1.path(), FileInfoType::FileInfoVideo);
    checkFileInfoDateTime(vid1.path(), "2018-01-01 01:01:01");

    VideoTestFile vid2;
    vid2.convert(workdir() / "test2.mov", VideoConvPreset::MOV);
    vid2.setCreationTime("2018-01-01 01:01:01Z");
    checkFileInfoType(vid2.path(), FileInfoType::FileInfoVideo);
    checkFileInfoDateTime(vid2.path(), "2018-01-01 01:01:01");

    VideoTestFile vid3;
    vid3.convert(workdir() / "test3.mkv", VideoConvPreset::MKV);
    vid3.setCreationTime("2018-01-01 01:01:01Z");
    checkFileInfoType(vid3.path(), FileInfoType::FileInfoVideo);
    checkFileInfoDateTime(vid3.path(), "2018-01-01 01:01:01");
}

TEST_F(FileInfoTests, invalidFileInfoVideo)
{
    VideoTestFile vid;
    vid.copy(workdir() / "test.mp4");
    vid.dropMetadata();
    // creation_time missing, should be invalid
    checkFileInvalid(vid.path());
}

TEST_F(FileInfoTests, unkownFileType)
{
    fs::path path = workdir() / "hello.txt";
    std::ofstream output(path);
    output << "Hello!";
    output.close();
    // file is neither image nor video, should be invalid
    checkFileInvalid(path);
}

} // namespace mediacopier::test
