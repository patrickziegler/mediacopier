/* Copyright (C) 2022 Patrick Ziegler <zipat@proton.me>
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

#include <mediacopier/jpeg.hpp>
#include <spdlog/spdlog.h>

extern "C"
{
#include <turbojpeg.h>
}

namespace fs = std::filesystem;

namespace mediacopier {

auto reset_exif_orientation(const fs::path& dest) noexcept -> bool
{
    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(dest.string());
        image->readMetadata();

        auto exif = image->exifData();
        exif["Exif.Image.Orientation"] = static_cast<int>(FileInfoImageJpeg::Orientation::ROT_0);

        image->setExifData(exif);
        image->writeMetadata();

    } catch(const std::exception& err) {
        spdlog::warn("Could not reset exif orientation tag (" + dest.string() + "): " + err.what());
        return false;
    }

    return true;
}

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
        spdlog::warn("Transformation not supported (%s with orientation %d)", file.path().string(), static_cast<int>(file.orientation()));
        return false;
    }

    // ----------------- read input file

    unsigned long inputBufSize = 0;
    unique_file_t inputFile(std::fopen(file.path().string().c_str(), "rb"), &std::fclose);

    if (!inputFile) {
        spdlog::warn("Could not open file for reading (%s)", file.path().string());
        return false;
    }

    if (fseek(inputFile.get(), 0, SEEK_END) < 0 || ((inputBufSize = ftell(inputFile.get())) < 0) || fseek(inputFile.get(), 0, SEEK_SET) < 0 || inputBufSize == 0) {
        spdlog::warn("Could not determine file size (%s)", file.path().string());
        return false;
    }

    unique_buf_t inputBufPtr((unsigned char*) tjAlloc(inputBufSize), &tjFree);

    if (!inputBufPtr) {
        spdlog::warn("Could not allocate input buffer (%s)", file.path().string());
        return false;
    }

    if (fread(inputBufPtr.get(), inputBufSize, 1, inputFile.get()) < 1) {
        spdlog::warn("Could not read from input file (%s)", file.path().string());
        return false;
    }

    // ----------------- execute transformation

    tjhandle tjInstance;

    if ((tjInstance = tjInitTransform()) == nullptr) {
        spdlog::warn("Could not initialize transformation (%s): %s", file.path().string(), tjGetErrorStr());
        return false;
    }

    unsigned long outputBufSize = 0;
    unsigned char * outputBuf = nullptr; // will be owned (and deleted) by outputBufPtr
    int flags = 0;

    if (tjTransform(tjInstance, inputBufPtr.get(), inputBufSize, 1, &outputBuf, &outputBufSize, &xform, flags) < 0) {
        tjDestroy(tjInstance);
        spdlog::warn("Could not execute transformation (%s): %s", file.path().string(), tjGetErrorStr());
        return false;
    }

    unique_buf_t outputBufPtr(outputBuf, &tjFree);

    // ----------------- write output file

    const char * c_path = reinterpret_cast<const char *>(dest.c_str());
    unique_file_t outputFile(std::fopen(c_path, "wb"), &std::fclose);

    if (!outputFile) {
        tjDestroy(tjInstance);
        spdlog::warn("Could not open file for writing (%s)", dest.string());
        return false;
    }

    if (fwrite(outputBufPtr.get(), outputBufSize, 1, outputFile.get()) < 1) {
        tjDestroy(tjInstance);
        spdlog::warn("Could not write to output file (%s)", dest.string());
        return false;
    }

    tjDestroy(tjInstance);

    return true;
}

} // namespace mediacopier
