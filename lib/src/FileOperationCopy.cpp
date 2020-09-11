/* Copyright (C) 2020 Patrick Ziegler
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

#include <mediacopier/Exceptions.hpp>
#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>
#include <mediacopier/FileOperationCopy.hpp>

extern "C"
{
#include <jpeglib.h>
#include "transupp.h"
}

#include <csetjmp>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
namespace mc = MediaCopier;

enum class JpegErrorValue {
    UnknownTransformation,
    ImageSizeError,
    JpegError,
    FileError,
};

struct JpegErrorCategory : public std::error_category
{
    const char* name() const noexcept override
    {
        return "JpegTransformation";
    }

    std::string message(int ev) const override
    {
        switch (static_cast<JpegErrorValue>(ev)) {
        case JpegErrorValue::UnknownTransformation:
            return "nonexistent airport name in request";
        case JpegErrorValue::ImageSizeError:
            return "request for a date from the past";
        case JpegErrorValue::JpegError:
            return "request for a date from the past";
        case JpegErrorValue::FileError:
            return "request for a date from the past";
        default:
            return "unrecognized error";
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
    // std::cerr << err->mgr.jpeg_message_table[err->mgr.msg_code] << std::endl;
    longjmp(err->env, 1);
}

static std::error_code jpeg_copy_rotated(const mc::FileInfoJpeg& file, const fs::path &dst)
{
    FILE *f_in, *f_out;
    jvirt_barray_ptr *c_coeff, *d_coeff;
    jpeg_decompress_struct c_info;
    jpeg_compress_struct d_info;
    jpeg_error c_err, d_err;
    jpeg_transform_info trans;
    JpegErrorCategory cat{};

    trans.trim = false;
    trans.crop = false;
    trans.force_grayscale = false;

    switch (file.orientation())
    {
    case mc::FileInfoJpeg::Orientation::ROT_180:
        trans.transform = JXFORM_ROT_180;
        break;
    case mc::FileInfoJpeg::Orientation::ROT_90:
        trans.transform = JXFORM_ROT_90;
        break;
    case mc::FileInfoJpeg::Orientation::ROT_270:
        trans.transform = JXFORM_ROT_270;
        break;
    default:
        return std::error_code{static_cast<int>(JpegErrorValue::UnknownTransformation), cat};
    }

    c_info.err = jpeg_std_error(&c_err.mgr);
    c_err.mgr.error_exit = jpeg_error_handler;

    if (setjmp(c_err.env)) {
        jpeg_destroy_decompress(&c_info);
        return std::error_code{static_cast<int>(JpegErrorValue::JpegError), cat};
    }

    d_info.err = jpeg_std_error(&d_err.mgr);
    d_err.mgr.error_exit = jpeg_error_handler;

    if (setjmp(d_err.env)) {
        jpeg_destroy_compress(&d_info);
        return std::error_code{static_cast<int>(JpegErrorValue::JpegError), cat};
    }

    jpeg_create_decompress(&c_info);
    jpeg_create_compress(&d_info);

    if ((f_in = fopen(file.path().c_str(), "rb")) == nullptr) {
        return std::error_code{static_cast<int>(JpegErrorValue::FileError), cat};
    }

    jpeg_stdio_src(&c_info, f_in);
    jcopy_markers_setup(&c_info, JCOPYOPT_ALL);
    jpeg_read_header(&c_info, true);
    jtransform_request_workspace(&c_info, &trans);

    if (c_info.image_width % 16 > 0 || c_info.image_height % 16 > 0) {
        jpeg_destroy_decompress(&c_info);
        jpeg_destroy_compress(&d_info);
        return std::error_code{static_cast<int>(JpegErrorValue::ImageSizeError), cat};
    }

    c_coeff = jpeg_read_coefficients(&c_info);
    jpeg_copy_critical_parameters(&c_info, &d_info);

    d_info.write_JFIF_header = false;
    d_coeff = jtransform_adjust_parameters(&c_info, &d_info, c_coeff, &trans);

    if ((f_out = fopen(dst.c_str(), "wb")) == nullptr) {
        fclose(f_in);
        return std::error_code{static_cast<int>(JpegErrorValue::FileError), cat};
    }

    jpeg_stdio_dest(&d_info, f_out);
    jpeg_write_coefficients(&d_info, d_coeff);
    jcopy_markers_execute(&c_info, &d_info, JCOPYOPT_ALL);

    jtransform_execute_transformation(&c_info, &d_info, c_coeff, &trans);

    jpeg_finish_compress(&d_info);
    jpeg_destroy_compress(&d_info);

    jpeg_finish_decompress(&c_info);
    jpeg_destroy_decompress(&c_info);

    fclose(f_in);
    fclose(f_out);

    return std::error_code{};
}

static bool check_equal(fs::path file1, fs::path file2)
{
    const std::streamsize buffer_size = 1024;
    std::vector<char> buffer(buffer_size, '\0');

    std::ifstream input1(file1.string(),  std::ios::in | std::ios::binary);
    std::ifstream input2(file2.string(),  std::ios::in | std::ios::binary);

    std::string chunk1, chunk2;

    while (!(input1.fail() || input2.fail())) {
        input1.read(buffer.data(), buffer_size);
        chunk1 = {buffer.begin(), buffer.begin() + input1.gcount()};

        input2.read(buffer.data(), buffer_size);
        chunk2 = {buffer.begin(), buffer.begin() + input2.gcount()};

        if (chunk1 != chunk2) {
            return false;
        }
    }
    return true;
}

void mc::FileOperationCopy::copyJpeg(const mc::FileInfoJpeg &file) const
{
    if (file.orientation() == mc::FileInfoJpeg::Orientation::ROT_0) {
        return copyFile(file);
    }

    auto dst = m_filePathFormat.createTemporaryPathFrom(file);
    fs::create_directories(dst.parent_path());

    auto err = jpeg_copy_rotated(file, dst);

    if (err.value() > 0) {
        if (err.value() == static_cast<int>(JpegErrorValue::ImageSizeError)) {
            // TODO: log this incident ( err.message() )
        }
        return copyFile(file);
    }

    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(dst);
        image->readMetadata();

        auto exif = image->exifData();
        exif["Exif.Image.Orientation"] = mc::FileInfoJpeg::Orientation::ROT_0;

        image->setExifData(exif);
        image->writeMetadata();

        FileInfoImage tmp(dst, exif);
        copyFile(tmp);

        fs::remove(dst, err);

    }  catch (const Exiv2::Error& err) {
        // TODO: log this incident
        return copyFile(file);
    }
}

void mc::FileOperationCopy::copyFile(const mc::AbstractFileInfo &file) const
{
    std::error_code err;
    unsigned int id = 0;
    while (true) {
        auto dst = m_filePathFormat.createPathFrom(file, id);
        if (!fs::exists(dst)) {
            fs::create_directories(dst.parent_path());
            fs::copy_file(file.path(), dst, err);
            if (err.value() > 0) {
                throw FileOperationError{err.message()};
            }
        }
        if (check_equal(file.path(), dst)) {
            return;
        }
        if (id == std::numeric_limits<unsigned int>::max()) {
            throw FileOperationError{"Unable to find unique filename"};
        }
        ++id;
    }
}

void mc::FileOperationCopy::visit(const mc::FileInfoImage &file) const
{
    copyFile(file);
}

void mc::FileOperationCopy::visit(const mc::FileInfoJpeg &file) const
{
    copyJpeg(file);
}

void mc::FileOperationCopy::visit(const mc::FileInfoVideo &file) const
{
    copyFile(file);
}
