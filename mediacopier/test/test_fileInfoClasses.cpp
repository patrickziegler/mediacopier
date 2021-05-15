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

#include <log4cplus/configurator.h>

namespace fs = std::filesystem;

class FileInfoTests : public ::testing::Test {
protected:
    void SetUp() override {
        log4cplus::BasicConfigurator log;
        log.configure();
    }
    fs::path test_data_dir = TEST_DATA_DIR;
};

TEST_F(FileInfoTests, fileInfoRot90)
{
    std::filesystem::path path = test_data_dir / "original/lena64_rot90.jpg";
    const auto& file = MediaCopier::FileInfoFactory::createFromPath(path);
    ASSERT_NE(file.get(), nullptr);
    const auto& fileJpeg = dynamic_cast<MediaCopier::FileInfoImageJpeg*>(file.get());
    ASSERT_EQ(fileJpeg->orientation(), MediaCopier::FileInfoImageJpeg::Orientation::ROT_90);
}
