/* Copyright (C) 2020 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/operation_copy_jpeg.hpp>

#include <mediacopier/file_info_video.hpp>
#include <spdlog/spdlog.h>

extern "C"
{
#include <turbojpeg.h>
}

namespace fs = std::filesystem;

namespace mediacopier {

constexpr static const auto upright = FileInfoImageJpeg::Orientation::ROT_0;

auto copy_rotate_jpeg(const FileInfoImageJpeg& file, const fs::path& dest) noexcept -> bool
{
    using unique_file_t = std::unique_ptr<std::FILE, decltype(&std::fclose)>;
    using unique_buf_t = std::unique_ptr<unsigned char, decltype(&tjFree)>;

    // ----------------- prepare transformation parameters

    tjtransform xform;
    memset(&xform, 0, sizeof(tjtransform));

    xform.options |= TJXOPT_PERFECT; // will cause tjTransform to return error when no perfect rotation is possible

    switch (static_cast<FileInfoImageJpeg::Orientation>(file.orientation()))
    {
    case FileInfoImageJpeg::Orientation::ROT_180:
        xform.op = TJXOP_ROT180;
        break;
    case FileInfoImageJpeg::Orientation::ROT_90:
        xform.op = TJXOP_ROT270;
        break;
    case FileInfoImageJpeg::Orientation::ROT_270:
        xform.op = TJXOP_ROT90;
        break;
    default:
        spdlog::warn("Transformation not supported ({0} with orientation {1:d})", file.path().string(), static_cast<int>(file.orientation()));
        return false;
    }

    // ----------------- read input file

    unsigned long inputBufSize = 0;
    unique_file_t inputFile(std::fopen(file.path().string().c_str(), "rb"), &std::fclose);

    if (!inputFile) {
        spdlog::warn("Could not open file for reading ({0})", file.path().string());
        return false;
    }

    if (fseek(inputFile.get(), 0, SEEK_END) < 0 || ((inputBufSize = ftell(inputFile.get())) < 0) || fseek(inputFile.get(), 0, SEEK_SET) < 0 || inputBufSize == 0) {
        spdlog::warn("Could not determine file size ({0})", file.path().string());
        return false;
    }

    unique_buf_t inputBufPtr((unsigned char*) tjAlloc(inputBufSize), &tjFree);

    if (!inputBufPtr) {
        spdlog::warn("Could not allocate input buffer ({0})", file.path().string());
        return false;
    }

    if (fread(inputBufPtr.get(), inputBufSize, 1, inputFile.get()) < 1) {
        spdlog::warn("Could not read from input file ({0})", file.path().string());
        return false;
    }

    // ----------------- execute transformation

    tjhandle tjInstance;

    if ((tjInstance = tjInitTransform()) == nullptr) {
        spdlog::warn("Could not initialize transformation ({0}): {1}", file.path().string(), tjGetErrorStr());
        return false;
    }

    unsigned long outputBufSize = 0;
    unsigned char * outputBuf = nullptr; // will be owned (and deleted) by outputBufPtr
    int flags = 0;

    if (tjTransform(tjInstance, inputBufPtr.get(), inputBufSize, 1, &outputBuf, &outputBufSize, &xform, flags) < 0) {
        tjDestroy(tjInstance);
        spdlog::warn("Could not execute transformation ({0}): {1}", file.path().string(), tjGetErrorStr());
        return false;
    }

    unique_buf_t outputBufPtr(outputBuf, &tjFree);

    // ----------------- write output file

    unique_file_t outputFile(std::fopen(dest.string().c_str(), "wb"), &std::fclose);

    if (!outputFile) {
        tjDestroy(tjInstance);
        spdlog::warn("Could not open file for writing ({0})", dest.string());
        return false;
    }

    if (fwrite(outputBufPtr.get(), outputBufSize, 1, outputFile.get()) < 1) {
        tjDestroy(tjInstance);
        spdlog::warn("Could not write to output file ({0})", dest.string());
        return false;
    }

    tjDestroy(tjInstance);

    return true;
}

auto FileOperationCopyJpeg::copyFileJpeg(const FileInfoImageJpeg& file) const -> void
{
    std::error_code err;
    fs::create_directories(m_destination.parent_path(), err);
    if (err.value()) {
        spdlog::warn("Could not create parent path ({0}): {1}", m_destination.parent_path().string(), err.message());
        return;
    }
    if (file.orientation() != upright && copy_rotate_jpeg(file, m_destination) && reset_exif_orientation(m_destination)) {
        return;
    }
    fs::copy_file(file.path(), m_destination, fs::copy_options::overwrite_existing, err);
    if (err.value()) {
        spdlog::warn("Could not copy jpeg file ({0}): {1}", file.path().string(), err.message());
    }
}

auto FileOperationCopyJpeg::visit(const FileInfoImage& file) -> void
{
    copyFile(file);
}

auto FileOperationCopyJpeg::visit(const FileInfoImageJpeg& file) -> void
{
    copyFileJpeg(file);
}

auto FileOperationCopyJpeg::visit(const FileInfoVideo& file) -> void
{
    copyFile(file);
}

} // namespace mediacopier
