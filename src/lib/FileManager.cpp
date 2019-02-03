#include "FileManager.hpp"
#include "ConfigManager.hpp"

#include <vector>
#include <sstream>
#include <algorithm>
#include <exception>
#include <ios>

#include <exiv2/exiv2.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace bf = boost::filesystem;
namespace bt = boost::posix_time;

std::locale FileManager::outputFormat;

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

const std::vector<std::locale> formats = {
    std::locale(std::locale(""), new bt::time_input_facet("%Y-%m-%d %H:%M:%s")),
    std::locale(std::locale(""), new bt::time_input_facet("%d-%m-%Y %H:%M:%s"))
};

inline bt::ptime parseDateStr(const std::string& str)
{
    bt::ptime pt;
    for (const std::locale& format : formats) {
        std::istringstream is(str);
        is.imbue(format);
        is >> pt;
        if (pt != bt::ptime()) {
            break;
        }
    }
    return pt;
}

bt::ptime FileManager::readExif(const bf::path& file) // http://www.exiv2.org/examples.html
{
    bt::ptime timestamp;
    Exiv2::ExifData exifData;
    std::unique_ptr<Exiv2::Image> image;
    std::string subsec;

    try {
        image = Exiv2::ImageFactory::open(file.string());
        image->readMetadata();
        exifData = image->exifData();
    } catch(const Exiv2::Error&) {
        return timestamp;
    }

    for (const std::string& keyDateTime : keysDateTime) {
        try {
            timestamp = parseDateStr(exifData[keyDateTime].toString());
        } catch (const Exiv2::Error&) {
            continue;
        }
    }

    if (timestamp == bt::ptime()) {
        return timestamp;
    }

    for (const std::string& keySubSec : keysSubSec) {
        try {
            subsec = exifData[keySubSec].toString();
            if (subsec.length() > 3) {
                return timestamp + bt::microseconds(std::stol(subsec));
            } else {
                return timestamp + bt::milliseconds(std::stol(subsec));
            }
        } catch (const Exiv2::Error&) {
            continue;
        }
    }

    return timestamp;
}

bt::ptime FileManager::readVideoMeta(const bf::path& file) // http://ffmpeg.org/doxygen/trunk/metadata_8c-example.html
{
    bt::ptime timestamp;
    AVFormatContext* fmt_ctx = nullptr;
    AVDictionaryEntry* tag = nullptr;

    if (!avformat_open_input(&fmt_ctx, file.c_str(), nullptr, nullptr)) {
        if ((tag = av_dict_get(fmt_ctx->metadata, "creation_time", nullptr, AV_DICT_IGNORE_SUFFIX))) {
            timestamp = parseDateStr(tag->value);
        }
    }

    avformat_close_input(&fmt_ctx);
    return timestamp;
}

FileManager::FileManager(const bf::path& file)
{
    outputFormat = std::locale(std::locale(""), new bt::time_facet(ConfigManager::instance().format.c_str()));

    if ((timestamp = readExif(file)) == bt::ptime()) {
        if ((timestamp = readVideoMeta(file)) == bt::ptime()) {
            throw std::invalid_argument("No metadata found in " + file.string());
        }
    }

    std::string ext = file.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    std::ostringstream os;
    os.imbue(outputFormat);
    os << timestamp;

    pathOld = file;
    pathNew = ConfigManager::instance().dirOutput / bf::path(os.str() + ext);
}

std::string FileManager::getPathOld() const
{
    return pathOld.string();
}

std::string FileManager::getPathNew() const
{
    return pathNew.string();
}

int FileManager::copy()
{
    bf::path pathNewDir(pathNew);
    pathNewDir.remove_filename();

    try {
        bf::create_directories(pathNewDir);
    } catch (const bf::filesystem_error&) {
        return 4;
    }

    if (ConfigManager::instance().flagOverride) {
        try {
            bf::copy_file(pathOld, pathNew, bf::copy_option::overwrite_if_exists);
        } catch (const bf::filesystem_error&) {
            return 1;
        }

    } else {
        try {
            bf::copy_file(pathOld, pathNew, bf::copy_option::fail_if_exists);
        } catch (const bf::filesystem_error&) {
            return 2;
        }
    }

    return 0;
}

int FileManager::move()
{
    int result = copy();

    if (!result) {
        try {
            remove(pathOld);
        } catch (const bf::filesystem_error&) {
            return 3;
        }
    }

    return result;
}

int FileManager::simulate()
{
    try {
        if (!ConfigManager::instance().flagOverride && exists(pathNew)) {
            return 2;
        }
    } catch (const bf::filesystem_error&) {
        return 1;
    }

    return 0;
}
