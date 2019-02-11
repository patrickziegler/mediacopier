#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"

#include <string>
#include <algorithm>
#include <csetjmp>

#include <exiv2/exiv2.hpp>

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

int jpeg_copy_rotated(const FileOperation& request)
{
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
        return 1;
    }

    c_info.err = jpeg_std_error(&c_err.mgr);
    c_err.mgr.error_exit = errorHandler;

    if (setjmp(c_err.env)) {
        jpeg_destroy_decompress(&c_info);
        return 4;
    }

    d_info.err = jpeg_std_error(&d_err.mgr);
    d_err.mgr.error_exit = errorHandler;

    if (setjmp(d_err.env)) {
        jpeg_destroy_compress(&d_info);
        return 5;
    }

    jpeg_create_decompress(&c_info);
    jpeg_create_compress(&d_info);

    if ((f_in = fopen(request.getPathOld().c_str(), "rb")) == nullptr) {
        return 2;
    }

    if ((f_out = fopen(request.getPathNew().c_str(), "wb")) == nullptr) {
        fclose(f_in);
        return 3;
    }

    jpeg_stdio_src(&c_info, f_in);
    jcopy_markers_setup(&c_info, JCOPYOPT_ALL);
    jpeg_read_header(&c_info, true);
    jtransform_request_workspace(&c_info, &trans);

    c_coeff = jpeg_read_coefficients(&c_info);
    jpeg_copy_critical_parameters(&c_info, &d_info);

    d_info.write_JFIF_header = false;
    d_coeff = jtransform_adjust_parameters(&c_info, &d_info, c_coeff, &trans);

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

inline int resetExifOrientation(const FileOperation& request)
{
    std::unique_ptr<Exiv2::Image> image;
    Exiv2::ExifData exifData;

    try {
        image = Exiv2::ImageFactory::open(request.getPathNew().string());
        image->readMetadata();

        exifData = image->exifData();
        exifData["Exif.Image.Orientation"] = 1;

        image->setExifData(exifData);
        image->writeMetadata();

    }  catch (const Exiv2::Error&) {
        return 1;
    }
    return 0;
}

FileOperationStrategy::~FileOperationStrategy() {}

int FileCopyOverwrite::execute(const FileOperation& request)
{
    bf::path pathNewDir(request.getPathNew());
    pathNewDir.remove_filename();

    try {
        bf::create_directories(pathNewDir);
    } catch (const bf::filesystem_error&) {
        return 4;
    }

    if (request.getMimeType() != "image/jpeg" || request.getOrientation() < 2 || jpeg_copy_rotated(request) || resetExifOrientation(request)) {
        try {
            bf::copy_file(request.getPathOld(), request.getPathNew(), bf::copy_option::overwrite_if_exists);
        } catch (const bf::filesystem_error&) {
            return 1;
        }
    }

    return 0;
}

int FileCopy::execute(const FileOperation& request)
{
    if (bf::exists(request.getPathNew())) {
        return 2;
    } else {
        return FileCopyOverwrite::execute(request);
    }
}

int FileMoveOverwrite::execute(const FileOperation& request)
{
    int result = FileCopyOverwrite::execute(request);
    if (!result) {
        try {
            bf::remove(request.getPathOld());
        } catch (const bf::filesystem_error&) {
            return 3;
        }
    }
    return result;
}

int FileMove::execute(const FileOperation& request)
{
    if (bf::exists(request.getPathNew())) {
        return 2;
    } else {
        return FileMoveOverwrite::execute(request);
    }
}

int FileSimulationOverwrite::execute(const FileOperation&)
{
    return 0;
}

int FileSimulation::execute(const FileOperation& request)
{
    std::lock_guard<std::mutex> lck(mtx);
    bf::path path = request.getPathNew();
    if (bf::exists(path) || (std::find(filesDone.begin(), filesDone.end(), path) != filesDone.end())) {
        return 2;
    } else {
        filesDone.push_back(request.getPathNew());
        return 0;
    }
}
