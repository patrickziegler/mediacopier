#define BOOST_TEST_MODULE test_FileOperation
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "FileOperation.hpp"
#include <iostream>

namespace bf = boost::filesystem;

struct FileOperationFixture
{
    FileOperationFixture()
    {
        FileOperation::setPathFormat("%Y/%Y-%m/%Y-%m-%d/TEST_%Y%m%d_%H%M%S_%f");
        FileOperation::setPathPrefix("data/dst");
    }
};

BOOST_FIXTURE_TEST_CASE(rot0_datetime_microsec, FileOperationFixture)
{
    bf::path pathOld("data/lena64_rot0.jpg");
    std::cout << "***** " << bf::absolute(pathOld).string() <<  std::endl;
    FileOperation op(pathOld);
    std::string pathNew("data/dst/2019/2019-02/2019-02-05/TEST_20190205_121032_123456.jpg");
    BOOST_CHECK_EQUAL(op.getMimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(op.getOrientation(), 1);
    BOOST_CHECK_EQUAL(op.getPathOld(), bf::absolute(pathOld));
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_FIXTURE_TEST_CASE(rot90_datetime_microsec, FileOperationFixture)
{
    FileOperation op(bf::path("data/lena64_rot90.jpg"));
    std::string pathNew("data/dst/2019/2019-02/2019-02-05/TEST_20190205_121132_012345.jpg");
    BOOST_CHECK_EQUAL(op.getMimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(op.getOrientation(), 8);
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_FIXTURE_TEST_CASE(rot180_datetime_microsec, FileOperationFixture)
{
    FileOperation op(bf::path("data/lena64_rot180.jpg"));
    std::string pathNew("data/dst/2019/2019-02/2019-02-05/TEST_20190205_121232_001234.jpg");
    BOOST_CHECK_EQUAL(op.getMimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(op.getOrientation(), 3);
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_FIXTURE_TEST_CASE(rot270_datetime_millisec, FileOperationFixture)
{
    FileOperation op(bf::path("data/lena64_rot270.jpg"));
    std::string pathNew("data/dst/2019/2019-02/2019-02-05/TEST_20190205_121332_123000.jpg");
    BOOST_CHECK_EQUAL(op.getMimeType(), "image/jpeg");
    BOOST_CHECK_EQUAL(op.getOrientation(), 6);
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_FIXTURE_TEST_CASE(video_h264_mp4, FileOperationFixture)
{
    FileOperation op(bf::path("data/roundhay_garden_scene.mp4"));
    std::string pathNew("data/dst/2018/2018-01/2018-01-01/TEST_20180101_010101_000000.mp4");
    BOOST_CHECK_EQUAL(op.getMimeType(), "");
    BOOST_CHECK_EQUAL(op.getOrientation(), 0);
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_FIXTURE_TEST_CASE(video_h264_mov, FileOperationFixture)
{
    FileOperation op(bf::path("data/roundhay_garden_scene.mov"));
    std::string pathNew("data/dst/2018/2018-01/2018-01-01/TEST_20180101_010101_000000.mov");
    BOOST_CHECK_EQUAL(op.getMimeType(), "");
    BOOST_CHECK_EQUAL(op.getOrientation(), 0);
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_FIXTURE_TEST_CASE(video_vp9_mkv, FileOperationFixture)
{
    FileOperation op(bf::path("data/roundhay_garden_scene.webm"));
    std::string pathNew("data/dst/2018/2018-01/2018-01-01/TEST_20180101_010101_000000.webm");
    BOOST_CHECK_EQUAL(op.getMimeType(), "");
    BOOST_CHECK_EQUAL(op.getOrientation(), 0);
    BOOST_CHECK_EQUAL(op.getPathNew().string(), pathNew);
}

BOOST_AUTO_TEST_CASE(fail_no_metadata)
{
    bf::path pathOld("data/lena64_rot270_copy.png");
    BOOST_WARN_THROW(FileOperation op(pathOld), std::invalid_argument);
}
