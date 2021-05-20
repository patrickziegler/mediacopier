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

#include <gtest/gtest.h>

#include <mediacopier/FileInfoFactory.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>

#include <date/date.h>
#include <log4cplus/configurator.h>

#include <chrono>

#include "FileInfoTypeDetector.hpp"

namespace fs = std::filesystem;

auto parse_timestamp(std::string timestamp) -> std::chrono::system_clock::time_point {
    std::istringstream ss{timestamp};
    std::chrono::system_clock::time_point ts;
    ss >> date::parse("%Y-%m-%d %H:%M:%S", ts);
    return ts; // right result was verified manually
};

namespace MediaCopier::Test {

class FileInfoTests : public ::testing::Test {
protected:
    void SetUp() override {
        log4cplus::BasicConfigurator log;
        log.configure();
    }
    fs::path test_data_dir = TEST_DATA_DIR;

    void checkNullptr(std::filesystem::path&& path)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_EQ(file.get(), nullptr);
    }

    void checkFileInfoType(std::filesystem::path&& path, FileInfoType type)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        file->accept(m_typeDetector);
        ASSERT_TRUE(m_typeDetector.lastType() == type);
    };

    void checkFileInfoJpeg(std::filesystem::path&& path, FileInfoImageJpeg::Orientation orientation,
                           std::string dateTimeOriginal)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        const auto& fileJpeg = dynamic_cast<FileInfoImageJpeg*>(file.get());
        ASSERT_EQ(fileJpeg->orientation(), orientation);

        const auto& timestamp = parse_timestamp(dateTimeOriginal);
        ASSERT_EQ(fileJpeg->timestamp(), timestamp);
    }

    void checkFileInfo(std::filesystem::path&& path, std::string dateTimeOriginal)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        const auto& timestamp = parse_timestamp(dateTimeOriginal);
        ASSERT_EQ(file->timestamp(), timestamp);
    }

private:
    FileInfoTypeDetector m_typeDetector;
};

TEST_F(FileInfoTests, validFileInfoImageJpeg)
{
    checkFileInfoType(test_data_dir / "original/lena64_rot0.jpg",
                      FileInfoType::FileInfoImageJpeg);

    checkFileInfoJpeg(test_data_dir / "original/lena64_rot0.jpg",
                      FileInfoImageJpeg::Orientation::ROT_0, "2019-02-05 12:10:32.123456");

    checkFileInfoJpeg(test_data_dir / "original/lena64_rot90.jpg",
                      FileInfoImageJpeg::Orientation::ROT_90, "2019-02-05 12:11:32");

    checkFileInfoJpeg(test_data_dir / "original/lena64_rot180.jpg",
                      FileInfoImageJpeg::Orientation::ROT_180, "2019-02-05 12:12:32.1234");

    checkFileInfoJpeg(test_data_dir / "original/lena64_rot270.jpg",
                      FileInfoImageJpeg::Orientation::ROT_270, "2019-02-05 12:13:32.123");
}

TEST_F(FileInfoTests, invalidFileInfoImageJpeg)
{
    checkFileInfoType(test_data_dir / "original/lena64_rot270_orientation_missing.jpg",
                      FileInfoType::FileInfoImage);

    checkNullptr(test_data_dir / "original/lena64_rot270_timestamp_missing.jpg");
}

TEST_F(FileInfoTests, validFileInfoVideo)
{
    checkFileInfoType(test_data_dir / "original/roundhay_garden_scene.mov",
                      FileInfoType::FileInfoVideo);

    checkFileInfoType(test_data_dir / "original/roundhay_garden_scene.mp4",
                      FileInfoType::FileInfoVideo);

    checkFileInfoType(test_data_dir / "original/roundhay_garden_scene.webm",
                      FileInfoType::FileInfoVideo);

    checkFileInfo(test_data_dir / "original/roundhay_garden_scene.mov", "2018-01-01 01:01:01");
    checkFileInfo(test_data_dir / "original/roundhay_garden_scene.mp4", "2018-01-01 01:01:01");
    checkFileInfo(test_data_dir / "original/roundhay_garden_scene.webm", "2018-01-01 01:01:01");
}

TEST_F(FileInfoTests, invalidFileInfoVideo)
{
    checkNullptr(test_data_dir / "original/video_timestamp_missing.mov");
    checkNullptr(test_data_dir / "original/video_broken.webm");
}

} // namespace MediaCopier::Test
