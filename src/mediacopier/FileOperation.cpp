#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"

#include <exiv2/exiv2.hpp>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace bf = boost::filesystem;
namespace bt = boost::posix_time;

std::shared_ptr<FileOperationStrategy> FileOperation::strategy;
boost::filesystem::path FileOperation::pathPrefix;
std::locale FileOperation::pathFormat;

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
    std::locale(std::locale(""), new bt::time_input_facet("%Y-%m-%d %H:%M:%s")),
    std::locale(std::locale(""), new bt::time_input_facet("%d-%m-%Y %H:%M:%s"))
};

int FileOperation::readExif() // http://www.exiv2.org/examples.html
{
    Exiv2::ExifData exifData;
    std::unique_ptr<Exiv2::Image> image;
    std::string subsec;
    bool parseFailed = true;

    try {
        image = Exiv2::ImageFactory::open(pathOld.string());
        image->readMetadata();
        exifData = image->exifData();
    } catch(const Exiv2::Error&) {
        return 1;
    }

    for (const std::string& keyDateTime : keysDateTime) {
        try {
            if (!setTimestamp(exifData[keyDateTime].toString())) {
                parseFailed = false;
                break;
            }
        } catch (const Exiv2::Error&) {
            continue;
        }
    }

    if (parseFailed) {
        return 1;
    }

    for (const std::string& keySubSec : keysSubSec) {
        try {
            subsec = exifData[keySubSec].toString();
            if (subsec.length() < 1) {
                continue;
            } else if (subsec.length() < 4) {
                timestamp + bt::milliseconds(std::stol(subsec));
            } else {
                timestamp + bt::microseconds(std::stol(subsec));
            }
        } catch (const Exiv2::Error&) {
            continue;
        }
    }

    try {
        mimeType = image->mimeType();
        orientation = static_cast<int>(exifData["Exif.Image.Orientation"].toLong());
    } catch (const Exiv2::Error&) {
        return 1;
    }

    return 0;
}

int FileOperation::readVideoMeta() // http://ffmpeg.org/doxygen/trunk/metadata_8c-example.html
{
    AVFormatContext* fmt_ctx = nullptr;
    AVDictionaryEntry* tag = nullptr;
    int result = 1;

    if (!avformat_open_input(&fmt_ctx, pathOld.c_str(), nullptr, nullptr)) {
        if ((tag = av_dict_get(fmt_ctx->metadata, "creation_time", nullptr, AV_DICT_IGNORE_SUFFIX))) {
            result = setTimestamp(tag->value);
        }
    }

    avformat_close_input(&fmt_ctx);
    return result;
}

int FileOperation::setTimestamp(const std::string& strDateTime)
{
    for (const std::locale& format : formatsDateTime) {
        std::istringstream buf(strDateTime);
        buf.imbue(format);
        buf >> timestamp;
        if (timestamp != bt::ptime()) {
            return 0;
        }
    }
    return 1;
}

void FileOperation::setStrategy(const std::shared_ptr<FileOperationStrategy>& strategy)
{
    FileOperation::strategy = strategy;
}

void FileOperation::setPathPrefix(const boost::filesystem::path& prefix)
{
    FileOperation::pathPrefix = prefix;
}

void FileOperation::setPathFormat(const std::string& format)
{
    FileOperation::pathFormat = std::locale(std::locale(""), new bt::time_facet(format.c_str()));
}

FileOperation::FileOperation(const boost::filesystem::path& file)
{
    pathOld = file;

    if (readExif() && readVideoMeta()) {
        throw std::invalid_argument("No metadata found in " + file.string());
    }

    std::string ext = file.extension().string();
    // std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    std::ostringstream buf;
    buf.imbue(pathFormat);
    buf << timestamp;

    pathNew = pathPrefix / bf::path(buf.str() + ext);
}

boost::filesystem::path FileOperation::getPathOld() const { return pathOld; }

boost::filesystem::path FileOperation::getPathNew() const { return pathNew; }

std::string FileOperation::getMimeType() const { return mimeType; }

int FileOperation::getOrientation() const { return orientation; }

std::string FileOperation::getLogMessage(int code) const
{
    std::ostringstream buf;

    buf << "File operation ";

    switch (code)
    {
    case 0:
        buf << "OK";
        break;
    case 1:
        buf << "FAILED: Could not copy / override file";
        break;
    case 2:
        buf << "FAILED: File already exists";
        break;
    case 3:
        buf << "FAILED: File could not be deleted";
        break;
    case 4:
        buf << "FAILED: Could not create directories";
        break;
    default:
        buf << "FAILED: Unknown error number";
        break;
    }

    buf << std::endl << pathOld.string()
        << std::endl << pathNew.string() << std::endl << std::endl;

    return buf.str();
}

int FileOperation::execute() { return strategy->execute(*this); }
