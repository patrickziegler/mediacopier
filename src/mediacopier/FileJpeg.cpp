// Copyright (C) 2019 Patrick Ziegler
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "FileJpeg.hpp"
#include "FileOperation.hpp"
#include <csetjmp>

extern "C"
{
#include <jpeglib.h>
#include "transupp.h"
}

namespace bf = boost::filesystem;

struct jpeg_error_struct {
    jpeg_error_mgr mgr;
    jmp_buf env;
};

using jpeg_error = struct jpeg_error_struct;
using jpeg_error_ptr = jpeg_error*;

[[noreturn]] void errorHandler(j_common_ptr c_info)
{
    jpeg_error_ptr err = reinterpret_cast<jpeg_error_ptr>(c_info->err);
    // std::cerr << err->mgr.jpeg_message_table[err->mgr.msg_code] << std::endl;
    longjmp(err->env, 1);
}

int jpeg_copy_rotated(const FileOperation& request, const bf::copy_option& copy_option)
{
    if (copy_option == bf::copy_option::fail_if_exists && bf::exists(request.getPathNew())) {
        return 1;
    }

    jpeg_decompress_struct c_info;
    jpeg_compress_struct d_info;
    jpeg_error c_err, d_err;
    jvirt_barray_ptr *c_coeff, *d_coeff;
    jpeg_transform_info trans;
    FILE *f_in, *f_out;

    trans.trim = false;
    trans.crop = false;
    trans.force_grayscale = false;

    switch (request.getOrientation())
    {
    case 6:
        trans.transform = JXFORM_ROT_90;
        break;
    case 3:
        trans.transform = JXFORM_ROT_180;
        break;
    case 8:
        trans.transform = JXFORM_ROT_270;
        break;
    default:
        return 2;
    }

    c_info.err = jpeg_std_error(&c_err.mgr);
    c_err.mgr.error_exit = errorHandler;

    if (setjmp(c_err.env)) {
        jpeg_destroy_decompress(&c_info);
        return 5;
    }

    d_info.err = jpeg_std_error(&d_err.mgr);
    d_err.mgr.error_exit = errorHandler;

    if (setjmp(d_err.env)) {
        jpeg_destroy_compress(&d_info);
        return 6;
    }

    jpeg_create_decompress(&c_info);
    jpeg_create_compress(&d_info);

    if ((f_in = fopen(request.getPathOld().c_str(), "rb")) == nullptr) {
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

    if ((f_out = fopen(request.getPathNew().c_str(), "wb")) == nullptr) {
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
