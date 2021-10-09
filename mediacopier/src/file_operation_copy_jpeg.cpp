/* Copyright (C) 2020-2021 Patrick Ziegler
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

#include <mediacopier/error.hpp>
#include <mediacopier/file_info_image.hpp>
#include <mediacopier/file_info_image_jpeg.hpp>
#include <mediacopier/file_info_video.hpp>
#include <mediacopier/file_operation_copy_jpeg.hpp>

#include <spdlog/spdlog.h>

extern "C"
{
#include <turbojpeg.h>
}

namespace fs = std::filesystem;

static auto jpeg_copy_rotated(const mediacopier::FileInfoImageJpeg& file, const fs::path& dst) -> void
{
    using unique_file_t = std::unique_ptr<std::FILE, decltype(&std::fclose)>;
    using unique_buf_t = std::unique_ptr<unsigned char, decltype(&tjFree)>;

    // ----------------- prepare transformation parameters

    tjtransform xform;
    memset(&xform, 0, sizeof(tjtransform));

    xform.options |= TJXOPT_PERFECT; // will cause tjTransform to return error when no perfect rotation is possible

    switch (static_cast<mediacopier::FileInfoImageJpeg::Orientation>(file.orientation()))
    {
    case mediacopier::FileInfoImageJpeg::Orientation::ROT_180:
        xform.op = TJXOP_ROT180;
        break;
    case mediacopier::FileInfoImageJpeg::Orientation::ROT_90:
        xform.op = TJXOP_ROT270;
        break;
    case mediacopier::FileInfoImageJpeg::Orientation::ROT_270:
        xform.op = TJXOP_ROT90;
        break;
    default:
        throw mediacopier::FileOperationError("Unknown jpeg transformation");
    }

    // ----------------- read input file

    unsigned long inputBufSize = 0;
    unique_file_t inputFile(std::fopen(file.path().string().c_str(), "rb"), &std::fclose);

    if (!inputFile) {
        throw mediacopier::FileOperationError("Could not open file for reading");
    }

    if (fseek(inputFile.get(), 0, SEEK_END) < 0 || ((inputBufSize = ftell(inputFile.get())) < 0) || fseek(inputFile.get(), 0, SEEK_SET) < 0 || inputBufSize == 0) {
        throw mediacopier::FileOperationError("Could not determinte file size");
    }

    unique_buf_t inputBufPtr((unsigned char*) tjAlloc(inputBufSize), &tjFree);

    if (!inputBufPtr) {
        throw mediacopier::FileOperationError("Could not allocate input buffer");
    }

    if (fread(inputBufPtr.get(), inputBufSize, 1, inputFile.get()) < 1) {
        throw mediacopier::FileOperationError("Could not read from input file");
    }

    // ----------------- execute transformation

    tjhandle tjInstance;

    if ((tjInstance = tjInitTransform()) == nullptr) {
        throw mediacopier::FileOperationError(std::string{"Could not initialize transformation: "} + tjGetErrorStr());
    }

    unsigned long outputBufSize = 0;
    unsigned char * outputBuf = nullptr; // will be owned (and deleted) by outputBufPtr
    int flags = 0;

    if (tjTransform(tjInstance, inputBufPtr.get(), inputBufSize, 1, &outputBuf, &outputBufSize, &xform, flags) < 0) {
        tjDestroy(tjInstance);
        throw mediacopier::FileOperationError(std::string{"Could not execute transformation: "} + tjGetErrorStr());
    }

    unique_buf_t outputBufPtr(outputBuf, &tjFree);

    // ----------------- write output file

    unique_file_t outputFile(std::fopen(dst.c_str(), "wb"), &std::fclose);

    if (!outputFile) {
        tjDestroy(tjInstance);
        throw mediacopier::FileOperationError("Could not open file for writing");
    }

    if (fwrite(outputBufPtr.get(), outputBufSize, 1, outputFile.get()) < 1) {
        tjDestroy(tjInstance);
        throw mediacopier::FileOperationError("Could not write to output file");
    }

    tjDestroy(tjInstance);
}

namespace mediacopier {

auto FileOperationCopyJpeg::copyJpeg(const FileInfoImageJpeg& file) const -> void
{
    if (file.orientation() == FileInfoImageJpeg::Orientation::ROT_0) {
        copyFile(file);
        return;
    }

    if (fs::exists(m_destination)) {
        spdlog::warn("Already exists: " + m_destination.filename().string());
    }

    try {
        fs::create_directories(m_destination.parent_path());
        jpeg_copy_rotated(file, m_destination);

        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(m_destination);
        image->readMetadata();

        auto exif = image->exifData();
        exif["Exif.Image.Orientation"] = static_cast<int>(FileInfoImageJpeg::Orientation::ROT_0);

        image->setExifData(exif);
        image->writeMetadata();

        return;

    } catch (const std::exception& err) {
        spdlog::warn(std::string{err.what()} + ": " + file.path().filename().string());
    }

    copyFile(file);
}

auto FileOperationCopyJpeg::visit(const FileInfoImage& file) -> void
{
    copyFile(file);
}

auto FileOperationCopyJpeg::visit(const FileInfoImageJpeg& file) -> void
{
    copyJpeg(file);
}
auto FileOperationCopyJpeg::visit(const FileInfoVideo& file) -> void
{
    copyFile(file);
}

} // namespace mediacopier
