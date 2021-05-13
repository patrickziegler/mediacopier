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

#include <mediacopier/exceptions.hpp>
#include <mediacopier/FileInfoImage.hpp>
#include <mediacopier/FileInfoImageJpeg.hpp>
#include <mediacopier/FileInfoVideo.hpp>
#include <mediacopier/FileOperationCopyJpeg.hpp>

#include <csetjmp>

extern "C"
{
#include "jpeglib.h"
#include "transupp.h"
}

namespace fs = std::filesystem;

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
        return "JpegError";
    }

    std::string message(int ev) const override
    {
        switch (static_cast<JpegErrorValue>(ev)) {
        case JpegErrorValue::UnknownTransformation:
            return "Unknown jpeg transformation";
        case JpegErrorValue::ImageSizeError:
            return "Image size not fit for transformation";
        case JpegErrorValue::JpegError:
            return "Error reading / writing jpeg coefficients";
        case JpegErrorValue::FileError:
            return "Error reading / writing image file";
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
    // std::cerr << err->mgr.jpeg_message_table[err->mgr.msg_code] << std::endl;
    longjmp(err->env, 1);
}

static std::error_code jpeg_copy_rotated(const MediaCopier::FileInfoImageJpeg& file, const fs::path &dst)
{
    FILE *f_in, *f_out;
    jvirt_barray_ptr *c_coeff, *d_coeff;
    jpeg_decompress_struct c_info;
    jpeg_compress_struct d_info;
    jpeg_error c_err, d_err;
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
        trans.transform = JXFORM_ROT_90;
        break;
    case MediaCopier::FileInfoImageJpeg::Orientation::ROT_270:
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

// TODO: warning: the use of `tmpnam' is dangerous, better use `mkstemp'

class TmpFile {
public:
    TmpFile() : m_path{fs::path{std::tmpnam(nullptr)}} {
        if (!fs::exists(m_path.parent_path())) {
            throw MediaCopier::FileOperationError("Parent path does not exist for " + m_path.string());
        }
    }

    ~TmpFile() {
        if (fs::exists(m_path)) {
            std::error_code err{}; // prevents exceptions from fs::remove
            fs::remove(m_path, err);
        }
    }

    auto path() -> const fs::path& {
        return m_path;
    }

private:
    fs::path m_path;
};

namespace MediaCopier {

void FileOperationCopyJpeg::copyJpeg(const FileInfoImageJpeg& file) const
{
    static constexpr const auto default_orientation = static_cast<int>(FileInfoImageJpeg::Orientation::ROT_0);

    if (file.orientation() == default_orientation) {
        return copyFile(file);
    }

    TmpFile tmpfile{};

    auto err = jpeg_copy_rotated(file, tmpfile.path());

    if (err.value() > 0) {
        if (err.value() == static_cast<int>(JpegErrorValue::ImageSizeError)) {
            // TODO: log this incident ( err.message() )
        }
        return copyFile(file);
    }

    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(tmpfile.path());
        image->readMetadata();

        auto exif = image->exifData();
        exif["Exif.Image.Orientation"] = default_orientation;

        image->setExifData(exif);
        image->writeMetadata();

        // TODO: copy from original file object instead
        FileInfoImageJpeg tmpinfo{tmpfile.path(), exif};

        return copyFile(tmpinfo);

    }  catch (const Exiv2::Error& err) {

        // TODO: log this incident

        return copyFile(file);
    }
}

void FileOperationCopyJpeg::visit(const FileInfoImage& file) const
{
    copyFile(file);
}

void FileOperationCopyJpeg::visit(const FileInfoImageJpeg& file) const
{
    copyJpeg(file);
}

void FileOperationCopyJpeg::visit(const FileInfoVideo& file) const
{
    copyFile(file);
}

}
