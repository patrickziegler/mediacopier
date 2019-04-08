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


#include "FileMeta.hpp"
#include <exiv2/exiv2.hpp>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace bf = boost::filesystem;
namespace bt = boost::posix_time;

// list of available keys: http://www.exiv2.org/tags.html

const std::vector<std::string> keysDateTime = {
    "Exif.Image.DateTime",
    "Exif.Image.DateTimeOriginal",
    "Exif.Photo.DateTimeOriginal",
    "Exif.Photo.DateTimeDigitized"
};

const std::vector<std::string> keysSubSec = {
    "Exif.Photo.SubSecTimeOriginal",
    "Exif.Photo.SubSecTimeDigitized",
    "Exif.Photo.SubSecTime"
};

const std::vector<std::locale> formatsDateTime = {
    std::locale(std::locale(""), new bt::time_input_facet("%Y:%m:%d %H:%M:%S")),
    std::locale(std::locale(""), new bt::time_input_facet("%Y-%m-%d %H:%M:%S")),
    std::locale(std::locale(""), new bt::time_input_facet("%d:%m:%Y %H:%M:%S")),
    std::locale(std::locale(""), new bt::time_input_facet("%d-%m-%Y %H:%M:%S"))
};

bt::ptime parseDateStr(const std::string& strDateTime)
{
    bt::ptime timestamp;

    for (const std::locale& format : formatsDateTime) {

        std::istringstream buf(strDateTime);
        buf.imbue(format);
        buf >> timestamp;

        if (!timestamp.is_not_a_date_time()) {
            break;
        }
    }

    return timestamp;
}

bt::ptime read_video_meta(const bf::path& file) // http://ffmpeg.org/doxygen/trunk/metadata_8c-example.html
{
    boost::posix_time::ptime timestamp;
    AVFormatContext* fmt_ctx = nullptr;
    AVDictionaryEntry* tag = nullptr;

    static bool LIBAV_INIT = [](){
        av_register_all();
        return true;
    } ();

    if (!avformat_open_input(&fmt_ctx, file.c_str(), nullptr, nullptr)) {

        if ((tag = av_dict_get(fmt_ctx->metadata, "creation_time", nullptr, AV_DICT_IGNORE_SUFFIX))) {

            timestamp = parseDateStr(tag->value);
        }
    }

    avformat_close_input(&fmt_ctx);

    if (timestamp.is_not_a_date_time()) {
        throw std::invalid_argument("No metadata found!");
    }

    return timestamp;
}

std::tuple<bt::ptime, std::string, int> read_exif_meta(const bf::path& file) // http://www.exiv2.org/examples.html
{
    std::unique_ptr<Exiv2::Image> image;
    Exiv2::ExifData exifData;
    bt::ptime timestamp;
    bt::ptime timestamp_subsec;
    std::string mimeType;
    int orientation;

    static bool EXIV2_INIT = [](){
        Exiv2::XmpParser::initialize();
        return true;
    } ();

    try {

        image = Exiv2::ImageFactory::open(file.string());
        image->readMetadata();
        exifData = image->exifData();

    } catch(const Exiv2::Error&) {
        throw std::invalid_argument("No metadata found!");
    }

    for (const std::string& keyDateTime : keysDateTime) {

        try {

            timestamp = parseDateStr(exifData[keyDateTime].toString());

            if (!timestamp.is_not_a_date_time()) {
                break;
            }

        } catch (const Exiv2::Error&) {
            continue;
        }
    }

    if (timestamp.is_not_a_date_time()) {
        throw std::invalid_argument("Not metadata found!");
    }

    std::string subsec_str;
    bt::time_duration subsec = bt::milliseconds(0);

    for (const std::string& keySubSec : keysSubSec) {

        try {

            subsec_str = exifData[keySubSec].toString();

            if (subsec_str.length() < 1) {
                continue;

            } else if (subsec_str.length() < 4) {
                subsec = bt::milliseconds(std::stol(subsec_str));
                break;

            } else {
                subsec = bt::microseconds(std::stol(subsec_str));
                break;
            }

        } catch (const Exiv2::Error&) {
            continue;
        }
    }

    timestamp_subsec = bt::ptime(timestamp + subsec);

    try {
        mimeType = image->mimeType();
        orientation = static_cast<int>(exifData["Exif.Image.Orientation"].toLong());

    } catch (const Exiv2::Error&) {
        throw std::invalid_argument("Not metadata found!");
    }

    if (timestamp_subsec.is_not_a_date_time()) {
        throw std::invalid_argument("Not metadata found!");
    }

    return std::make_tuple(timestamp_subsec, mimeType, orientation);
}

int reset_exif_orientation(const bf::path& file)
{
    std::unique_ptr<Exiv2::Image> image;
    Exiv2::ExifData exifData;

    try {
        image = Exiv2::ImageFactory::open(file.string());
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
