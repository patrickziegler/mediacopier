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

#pragma once

#include <gtest/gtest.h>

#include <mediacopier/FileInfoFactory.hpp>

#include <chrono>

#include <date/date.h>
#include <log4cplus/configurator.h>

#include "FileInfoTypeDetector.hpp"

namespace fs = std::filesystem;

static auto parse_timestamp(std::string timestamp) -> std::chrono::system_clock::time_point {
    std::istringstream ss{timestamp};
    std::chrono::system_clock::time_point ts;
    ss >> date::parse("%Y-%m-%d %H:%M:%S", ts);
    return ts; // right result was verified manually
};

namespace MediaCopier::Test {

class CommonTestFixtures : public ::testing::Test {
protected:
    fs::path test_data_dir = TEST_DATA_DIR;
    fs::path test_data_dir_orig = test_data_dir / "original";

    void SetUp() override {
        log4cplus::BasicConfigurator log;
        log.configure();
    }

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

} // namespace MediaCopier::Test
