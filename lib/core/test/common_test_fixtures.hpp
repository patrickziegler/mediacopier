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

#pragma once

#include <gtest/gtest.h>
#include <mediacopier/abstract_operation.hpp>
#include <mediacopier/file_info_factory.hpp>
#include <mediacopier/file_info_image_jpeg.hpp>

#include <chrono>
#include <filesystem>
#include <random>

namespace fs = std::filesystem;

static auto parse_timestamp(std::string timestamp)
{
    std::istringstream ss { timestamp };
    std::chrono::system_clock::time_point ts;
    ss >> std::chrono::parse("%Y-%m-%d %H:%M:%S", ts);
    return ts; // right result was verified manually
};

static auto generate_random_hash(size_t hash_len = 12)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 15);
    std::stringstream ss;
    for (size_t i = 0; i < hash_len; ++i) {
        ss << std::hex << dist(gen);
    }
    return ss.str();
}

static auto rename_append_random_hash(const fs::path& path, size_t hash_len = 12)
{
    if (!fs::exists(path)) {
        throw std::runtime_error("file does not exist: " + path.string());
    }
    std::string hash = generate_random_hash(hash_len);
    fs::path path_new = path.parent_path() / (path.stem().string() + "-" + hash + path.extension().string());
    fs::rename(path, path_new);
    return path_new;
}

static void run(const std::string&& cmd)
{
    const auto res = std::system(cmd.c_str());
    if (res) {
        throw std::runtime_error(std::format("failed to run: '{}', result: {}", cmd, res));
    }
}

namespace mediacopier::test {

enum class Rotation {
    R90,
    R180,
    R270,
};

enum class VideoConvPreset {
    MOV,
    MKV
};

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class TestFile {
public:
    explicit TestFile(fs::path path)
        : m_path(std::move(path))
    {
    }
    virtual ~TestFile()
    {
        std::error_code ec;
        if (fs::exists(m_path, ec) && m_path.parent_path() != fs::path { TEST_DATA_DIR }) {
            fs::remove(m_path, ec);
        }
    }
    void copy(const fs::path& dst)
    {
        fs::copy_file(m_path, dst);
        m_path = dst;
    }
    const fs::path& path() const
    {
        return m_path;
    }
    fs::path m_path;
};

class ImageTestFile : public TestFile {
public:
    ImageTestFile()
        : TestFile(fs::path { TEST_DATA_DIR } / "lena512color.tiff")
    {
    }
    void convert(const fs::path& dst, const std::string_view size = "64x64", size_t quality = 25)
    {
        std::string cmd = "magick";
        if (std::system("which magick")) {
            cmd = "convert";
        }
        run(std::format("{} {} -resize {} -quality {} -colorspace GRAY {}", cmd, m_path.string(), size, quality, dst.string()));
        m_path = dst;
    }
    void rotate(const Rotation r, bool perfect = true, bool optimize = true) const
    {
        std::string optPerfect = "";
        if (perfect) {
            optPerfect = " -perfect";
        }
        std::string optOptimize = "";
        if (optimize) {
            optOptimize = " -optimize";
        }
        // https://stackoverflow.com/questions/14612809/jpegtran-exe-not-correctly-rotating-image
        auto tmp_path = rename_append_random_hash(m_path);
        switch (r) {
        case Rotation::R90:
            run(std::format("jpegtran{}{} -rotate {} {} > {}", optPerfect, optOptimize, 90, tmp_path.string(), m_path.string()));
            break;
        case Rotation::R180:
            run(std::format("jpegtran{}{} -rotate {} {} > {}", optPerfect, optOptimize, 180, tmp_path.string(), m_path.string()));
            break;
        case Rotation::R270:
            run(std::format("jpegtran{}{} -rotate {} {} > {}", optPerfect, optOptimize, 270, tmp_path.string(), m_path.string()));
            break;
        }
        run(std::format("exiftool -overwrite_original -TagsFromFile {} -all:all {}", tmp_path.string(), m_path.string()));
        fs::remove(tmp_path);
    }
    void setExif(const FileInfoImageJpeg::Orientation o) const
    {
        // https://www.daveperrett.com/articles/2012/07/28/exif-orientation-handling-is-a-ghetto
        switch (o) {
        case FileInfoImageJpeg::Orientation::ROT_0:
            run(std::format("exiftool -overwrite_original -n -Orientation=1 {}", m_path.string()));
            break;
        case FileInfoImageJpeg::Orientation::ROT_90:
            run(std::format("exiftool -overwrite_original -n -Orientation=8 {}", m_path.string()));
            break;
        case FileInfoImageJpeg::Orientation::ROT_180:
            run(std::format("exiftool -overwrite_original -n -Orientation=3 {}", m_path.string()));
            break;
        case FileInfoImageJpeg::Orientation::ROT_270:
            run(std::format("exiftool -overwrite_original -n -Orientation=6 {}", m_path.string()));
            break;
        default:
            // mirrored not supported
            break;
        }
    }
    void setExif(const std::string_view field, const std::string_view value) const
    {
        run(std::format("exiftool -overwrite_original -{}=\"{}\" {}", field, value, m_path.string()));
    }
};

class VideoTestFile : public TestFile {
public:
    VideoTestFile()
        : TestFile(fs::path { TEST_DATA_DIR } / "roundhay_garden_scene.mp4")
    {
    }
    void convert(const fs::path& dst, VideoConvPreset preset)
    {
        switch (preset) {
        case VideoConvPreset::MOV:
            run(std::format("ffmpeg -i {} -map_metadata 0 -codec copy {}", m_path.string(), dst.string()));
            break;
        case VideoConvPreset::MKV:
            run(std::format("ffmpeg -i {} -map_metadata 0 -codec vp9 -crf 63 {}", m_path.string(), dst.string()));
            break;
        }
        m_path = dst;
    }
    void setCreationTime(const std::string_view datetime) const
    {
        auto tmp_path = rename_append_random_hash(m_path);
        run(std::format("ffmpeg -i {} -metadata creation_time=\"{}\" {}", tmp_path.string(), datetime, m_path.string()));
        fs::remove(tmp_path);
    }
    void dropMetadata() const
    {
        auto tmp_path = rename_append_random_hash(m_path);
        run(std::format("ffmpeg -i {} -c copy -map_metadata -1 {}", tmp_path.string(), m_path.string()));
        fs::remove(tmp_path);
    }
};

enum class FileInfoType {
    None,
    FileInfoImage,
    FileInfoImageJpeg,
    FileInfoVideo,
};

class FileInfoTypeDetector : public AbstractFileOperation {
public:
    auto visit(const FileInfoImage& /*file*/) -> void override
    {
        m_lastType = FileInfoType::FileInfoImage;
    }

    auto visit(const FileInfoImageJpeg& /*file*/) -> void override
    {
        m_lastType = FileInfoType::FileInfoImageJpeg;
    }

    auto visit(const FileInfoVideo& /*file*/) -> void override
    {
        m_lastType = FileInfoType::FileInfoVideo;
    }

    auto lastType() const -> FileInfoType
    {
        return m_lastType;
    }

private:
    FileInfoType m_lastType = FileInfoType::None;
};

class CommonTestFixtures : public ::testing::Test {
public:
    CommonTestFixtures()
    {
        m_workdir = fs::temp_directory_path() / ("mediacopier-" + generate_random_hash());
        fs::create_directories(m_workdir);
    }
    ~CommonTestFixtures() noexcept override
    {
        std::error_code ec;
        if (fs::exists(m_workdir, ec)) {
            fs::remove_all(m_workdir, ec);
        }
    }
    const fs::path& workdir() const
    {
        return m_workdir;
    }
    void checkFileInvalid(const std::filesystem::path& path) const
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_EQ(file.get(), nullptr);
    }
    void checkFileInfoType(const std::filesystem::path& path, FileInfoType type)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        file->accept(m_typeDetector);
        ASSERT_TRUE(m_typeDetector.lastType() == type);
    };
    void checkFileInfoJpegOrientation(const std::filesystem::path& path, FileInfoImageJpeg::Orientation orientation)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        const auto& fileJpeg = dynamic_cast<FileInfoImageJpeg*>(file.get());
        ASSERT_EQ(fileJpeg->orientation(), orientation);
    }
    void checkFileInfoJpegAttrs(const std::filesystem::path& path, FileInfoImageJpeg::Orientation orientation, std::string dateTimeOriginal)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        const auto& fileJpeg = dynamic_cast<FileInfoImageJpeg*>(file.get());
        ASSERT_EQ(fileJpeg->orientation(), orientation);

        const auto& timestamp = parse_timestamp(dateTimeOriginal);
        ASSERT_EQ(fileJpeg->timestamp(), timestamp);
    }
    void checkFileInfoDateTime(const std::filesystem::path& path, std::string dateTime)
    {
        const auto& file = FileInfoFactory::createFromPath(path);
        ASSERT_NE(file.get(), nullptr);

        const auto& timestamp = parse_timestamp(dateTime);
        ASSERT_EQ(file->timestamp(), timestamp);
    }

private:
    fs::path m_workdir;
    FileInfoTypeDetector m_typeDetector;
};

} // namespace mediacopier::test
