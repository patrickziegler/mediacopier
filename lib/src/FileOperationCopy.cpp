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

namespace fs = std::filesystem;
namespace mc = MediaCopier;

struct jpeg_error_struct {
    jpeg_error_mgr mgr;
    jmp_buf env;
};

using jpeg_error = struct jpeg_error_struct;
using jpeg_error_ptr = jpeg_error*;

static void jpeg_error_handler(j_common_ptr c_info)
{
    jpeg_error_ptr err = reinterpret_cast<jpeg_error_ptr>(c_info->err);
    std::cerr << err->mgr.jpeg_message_table[err->mgr.msg_code] << std::endl;
    longjmp(err->env, 1);
}

static int jpeg_copy_rotated(const mc::FileInfoJpeg& file, const fs::path &dst)
{
    FILE *f_in, *f_out;
    jvirt_barray_ptr *c_coeff, *d_coeff;
    jpeg_decompress_struct c_info;
    jpeg_compress_struct d_info;
    jpeg_error c_err, d_err;
    jpeg_transform_info trans;

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
        return 2;
    }

    c_info.err = jpeg_std_error(&c_err.mgr);
    c_err.mgr.error_exit = jpeg_error_handler;

    if (setjmp(c_err.env)) {
        jpeg_destroy_decompress(&c_info);
        return 5;
    }

    d_info.err = jpeg_std_error(&d_err.mgr);
    d_err.mgr.error_exit = jpeg_error_handler;

    if (setjmp(d_err.env)) {
        jpeg_destroy_compress(&d_info);
        return 6;
    }

    jpeg_create_decompress(&c_info);
    jpeg_create_compress(&d_info);

    if ((f_in = fopen(file.path().c_str(), "rb")) == nullptr) {
        return 3;
    }

    jpeg_stdio_src(&c_info, f_in);
    jcopy_markers_setup(&c_info, JCOPYOPT_ALL);
    jpeg_read_header(&c_info, true);
    jtransform_request_workspace(&c_info, &trans);

    if (c_info.image_width % 16 > 0 || c_info.image_height % 16 > 0) {
        jpeg_destroy_decompress(&c_info);
        jpeg_destroy_compress(&d_info);
        return 7;
    }

    c_coeff = jpeg_read_coefficients(&c_info);
    jpeg_copy_critical_parameters(&c_info, &d_info);

    d_info.write_JFIF_header = false;
    d_coeff = jtransform_adjust_parameters(&c_info, &d_info, c_coeff, &trans);

    if ((f_out = fopen(dst.c_str(), "wb")) == nullptr) {
        fclose(f_in);
        return 4;
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

    return 0;
}

int mc::FileOperationCopy::copyJpeg(const mc::FileInfoJpeg &file) const
{
    if (file.orientation() == mc::FileInfoJpeg::Orientation::ROT_0) {
        return copyFile(file);
    }

    auto dst = m_filePathFormat.createTemporaryPathFrom(file);
    fs::create_directories(dst.parent_path());
    auto ret = jpeg_copy_rotated(file, dst);

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
        fs::remove(dst);

    }  catch (const Exiv2::Error&) {
        return 1;
    }

    return ret;
}

int mc::FileOperationCopy::copyFile(const mc::AbstractFileInfo &file) const
{
    auto dst = m_filePathFormat.createPathFrom(file);
    fs::create_directories(dst.parent_path());
    fs::copy_file(file.path(), dst, fs::copy_options::skip_existing);
    return 0;
}

int mc::FileOperationCopy::visit(const mc::FileInfoImage &file) const
{
    return copyFile(file);
}

int mc::FileOperationCopy::visit(const mc::FileInfoJpeg &file) const
{
    return copyJpeg(file);
}

int mc::FileOperationCopy::visit(const mc::FileInfoVideo &file) const
{
    return copyFile(file);
}
