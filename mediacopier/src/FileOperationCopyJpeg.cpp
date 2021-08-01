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

#include <mediacopier/Error.hpp>
#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>

#include <log4cplus/loggingmacros.h>

#include <csetjmp>
#include <iostream>

extern "C"
{
#include "jpeglib.h"
#include "transupp.h"
}

namespace fs = std::filesystem;

enum class JpegErrorValue {
    UnknownTransformation,
    ImageSizeError,
    IOError,
};

struct JpegErrorCategory : public std::error_category
{
    const char* name() const noexcept override
    {
        return "JpegError";
    }

    std::string message(int ev) const override
    {
        switch (static_cast<JpegErrorValue>(ev)) {
        case JpegErrorValue::UnknownTransformation:
            return "Unknown jpeg transformation";
        case JpegErrorValue::ImageSizeError:
            return "Image size not fit for transformation";
        case JpegErrorValue::IOError:
            return "Error reading / writing jpeg coefficients or image file";
        default:
            return "Unknown error";
        }
    }
};

struct jpeg_error_struct {
    jpeg_error_mgr mgr;
    jmp_buf env;
};

using jpeg_error = struct jpeg_error_struct;
using jpeg_error_ptr = jpeg_error*;

static void jpeg_error_handler(j_common_ptr c_info)
{
    jpeg_error_ptr err = reinterpret_cast<jpeg_error_ptr>(c_info->err);
    std::cerr << "jpeg_error_handler: " << err->mgr.jpeg_message_table[err->mgr.msg_code] << std::endl;
    longjmp(err->env, 1);
}

static std::error_code jpeg_copy_rotated(const MediaCopier::FileInfoImageJpeg& file, const fs::path& dst)
{
    FILE *f_in, *f_out;
    jvirt_barray_ptr *c_coeff, *d_coeff;
    jpeg_decompress_struct c_info;
    jpeg_compress_struct d_info;
    jpeg_error_mgr c_err, d_err;
    jpeg_transform_info trans;

    JpegErrorCategory cat;

    trans.trim = false;
    trans.crop = false;
    trans.force_grayscale = false;

    switch (static_cast<MediaCopier::FileInfoImageJpeg::Orientation>(file.orientation()))
    {
    case MediaCopier::FileInfoImageJpeg::Orientation::ROT_180:
        trans.transform = JXFORM_ROT_180;
        break;
    case MediaCopier::FileInfoImageJpeg::Orientation::ROT_90:
        trans.transform = JXFORM_ROT_270;
        break;
    case MediaCopier::FileInfoImageJpeg::Orientation::ROT_270:
        trans.transform = JXFORM_ROT_90;
        break;
    default:
        return std::error_code{static_cast<int>(JpegErrorValue::UnknownTransformation), cat};
    }

    // create source struct (decompress) and error handlers

    c_info.err = jpeg_std_error(&c_err);
    c_err.error_exit = jpeg_error_handler;

    jpeg_create_decompress(&c_info);

    // create destination struct (compress) and error handlers

    d_info.err = jpeg_std_error(&d_err);
    d_err.error_exit = jpeg_error_handler;

    jpeg_create_compress(&d_info);

    // open file, read header and check image size

    if ((f_in = fopen(file.path().c_str(), "rb")) == nullptr) {
        jpeg_destroy_decompress(&c_info);
        jpeg_destroy_compress(&d_info);
        return std::error_code{static_cast<int>(JpegErrorValue::IOError), cat};
    }

    jpeg_stdio_src(&c_info, f_in);
    jcopy_markers_setup(&c_info, JCOPYOPT_ALL);
    jpeg_read_header(&c_info, true);

    if (c_info.image_width % 16 > 0 || c_info.image_height % 16 > 0) {
        fclose(f_in);
        jpeg_destroy_decompress(&c_info);
        jpeg_destroy_compress(&d_info);
        return std::error_code{static_cast<int>(JpegErrorValue::ImageSizeError), cat};
    }

    // do transform

    jtransform_request_workspace(&c_info, &trans);

    c_coeff = jpeg_read_coefficients(&c_info);
    jpeg_copy_critical_parameters(&c_info, &d_info);

    d_info.write_JFIF_header = false;
    d_coeff = jtransform_adjust_parameters(&c_info, &d_info, c_coeff, &trans);

    if ((f_out = fopen(dst.c_str(), "wb")) == nullptr) {
        fclose(f_in);
        return std::error_code{static_cast<int>(JpegErrorValue::IOError), cat};
    }

    jpeg_stdio_dest(&d_info, f_out);
    jpeg_write_coefficients(&d_info, d_coeff);
    jcopy_markers_execute(&c_info, &d_info, JCOPYOPT_ALL);

    jtransform_execute_transformation(&c_info, &d_info, c_coeff, &trans);

    jpeg_finish_compress(&d_info);
    jpeg_destroy_compress(&d_info);

    jpeg_finish_decompress(&c_info);
    jpeg_destroy_decompress(&c_info);

    fclose(f_out);
    fclose(f_in);

    return {};
}

namespace MediaCopier {

void FileOperationCopyJpeg::copyJpeg(const FileInfoImageJpeg& file) const
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("copyJpeg"));

    if (file.orientation() == FileInfoImageJpeg::Orientation::ROT_0) {
        return copyFile(file);
    }

    if (fs::exists(m_destination)) {
        LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT("Already exists: " + m_destination.filename().string()));
    }

    fs::create_directories(m_destination.parent_path());
    auto err = jpeg_copy_rotated(file, m_destination);

    if (err.value() > 0) {
        LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(err.message() + ": " + file.path().filename().string()) + " -> " + m_destination.filename().string());
        return copyFile(file);
    }

    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(m_destination);
        image->readMetadata();

        auto exif = image->exifData();
        exif["Exif.Image.Orientation"] = static_cast<int>(FileInfoImageJpeg::Orientation::ROT_0);

        image->setExifData(exif);
        image->writeMetadata();

    }  catch (const Exiv2::Error& err) {
        LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(std::string{err.what()} + ": " + file.path().filename().string()));
        return copyFile(file);
    }
}

void FileOperationCopyJpeg::visit(const FileInfoImage& file)
{
    copyFile(file);
}

void FileOperationCopyJpeg::visit(const FileInfoImageJpeg& file)
{
    copyJpeg(file);
}

void FileOperationCopyJpeg::visit(const FileInfoVideo& file)
{
    copyFile(file);
}

} // namespace MediaCopier
