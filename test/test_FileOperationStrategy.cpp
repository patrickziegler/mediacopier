#define BOOST_TEST_MODULE test_FileOperationStrategy
#define BOOST_TEST_DYN_LINK

#include "FileOperation.hpp"
#include "FileOperationStrategy.hpp"
#include <boost/test/unit_test.hpp>

namespace bf = boost::filesystem;
using strategy_ptr = std::shared_ptr<FileOperationStrategy>;

struct FileOperationStrategyFixture
{
    FileOperationStrategyFixture()
    {
        FileOperation::setPathFormat("TEST_%Y%m%d_%H%M%S_%f");
        FileOperation::setPathPrefix("tmp");
        FileOperation::setStrategy(strategy_ptr(new FileCopy()));
    }
    ~FileOperationStrategyFixture()
    {
        bf::remove_all("tmp");
    }
};

BOOST_FIXTURE_TEST_CASE(rot0_good_size, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena64_rot0.jpg"));
    op1.execute();
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    FileOperation op2(op1.getPathNew());
    BOOST_CHECK_EQUAL(op2.getOrientation(), 1);
}

BOOST_FIXTURE_TEST_CASE(copy_rot90_good_size, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena64_rot90.jpg"));
    op1.execute();
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    FileOperation op2(op1.getPathNew());
    BOOST_CHECK_EQUAL(op2.getOrientation(), 1);
}

BOOST_FIXTURE_TEST_CASE(copy_rot180_good_size, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena64_rot180.jpg"));
    op1.execute();
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    FileOperation op2(op1.getPathNew());
    BOOST_CHECK_EQUAL(op2.getOrientation(), 1);
}

BOOST_FIXTURE_TEST_CASE(copy_rot270_good_size, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena64_rot270.jpg"));
    op1.execute();
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    FileOperation op2(op1.getPathNew());
    BOOST_CHECK_EQUAL(op2.getOrientation(), 1);
}

BOOST_FIXTURE_TEST_CASE(copy_rot90_bad_size, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena50_rot90.jpg"));
    op1.execute();
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    FileOperation op2(op1.getPathNew());
    BOOST_CHECK_EQUAL(op2.getOrientation(), 8);
}

BOOST_FIXTURE_TEST_CASE(copy_rot90_tiff, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena16_rot90.tiff"));
    op1.execute();
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    FileOperation op2(op1.getPathNew());
    BOOST_CHECK_EQUAL(op2.getOrientation(), 8);
}

BOOST_FIXTURE_TEST_CASE(copy_overwrite_video, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/roundhay_garden_scene.mp4"));
    BOOST_CHECK_EQUAL(op1.execute(), 0);
    BOOST_CHECK(bf::exists(op1.getPathOld()));
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    BOOST_CHECK_EQUAL(op1.execute(), -1);

    bf::copy_option opt = bf::copy_option::overwrite_if_exists;
    bf::copy_file(bf::path("data/roundhay_garden_scene.webm"), op1.getPathNew(), opt);
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    BOOST_CHECK_EQUAL(op1.execute(), 2);

    FileOperation::setStrategy(strategy_ptr(new FileCopyOverwrite()));
    BOOST_CHECK_EQUAL(op1.execute(), 0);

    BOOST_CHECK(bf::exists(op1.getPathOld()));
    BOOST_CHECK(bf::exists(op1.getPathNew()));
}

BOOST_FIXTURE_TEST_CASE(copy_overwrite_rot180, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena64_rot180.jpg"));
    BOOST_CHECK_EQUAL(op1.execute(), 0);
    BOOST_CHECK(bf::exists(op1.getPathOld()));
    BOOST_CHECK(bf::exists(op1.getPathNew()));
    BOOST_CHECK_EQUAL(op1.execute(), -1);

    bf::copy_option opt = bf::copy_option::overwrite_if_exists;
    bf::copy_file(bf::path("data/lena64_rot90.jpg"), op1.getPathNew(), opt);
    BOOST_CHECK_EQUAL(op1.execute(), 2);

    FileOperation::setStrategy(strategy_ptr(new FileCopyOverwrite()));
    BOOST_CHECK_EQUAL(op1.execute(), 0);

    BOOST_CHECK(bf::exists(op1.getPathOld()));
    BOOST_CHECK(bf::exists(op1.getPathNew()));
}

BOOST_FIXTURE_TEST_CASE(move_overwrite_video, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/roundhay_garden_scene.webm"));
    BOOST_CHECK_EQUAL(op1.execute(), 0);
    BOOST_CHECK_EQUAL(bf::exists(op1.getPathOld()), true);
    BOOST_CHECK_EQUAL(bf::exists(op1.getPathNew()), true);

    FileOperation::setPathFormat("TEST2_%Y%m%d_%H%M%S_%f");
    FileOperation op2(op1.getPathNew());
    bf::copy_option opt = bf::copy_option::overwrite_if_exists;
    bf::copy_file(op2.getPathOld(), op2.getPathNew(), opt);
    BOOST_CHECK_EQUAL(op2.execute(), -1);

    FileOperation::setStrategy(strategy_ptr(new FileMoveOverwrite()));
    BOOST_CHECK_EQUAL(op2.execute(), -1);
    BOOST_CHECK_EQUAL(bf::exists(op2.getPathOld()), false);
    BOOST_CHECK_EQUAL(bf::exists(op2.getPathNew()), true);
}

BOOST_FIXTURE_TEST_CASE(move_overwrite_rot270, FileOperationStrategyFixture)
{
    FileOperation op1(bf::path("data/lena64_rot270.jpg"));
    BOOST_CHECK_EQUAL(op1.execute(), 0);
    BOOST_CHECK_EQUAL(bf::exists(op1.getPathOld()), true);
    BOOST_CHECK_EQUAL(bf::exists(op1.getPathNew()), true);

    FileOperation::setPathFormat("TEST2_%Y%m%d_%H%M%S_%f");
    FileOperation op2(op1.getPathNew());
    bf::copy_option opt = bf::copy_option::overwrite_if_exists;
    bf::copy_file(op2.getPathOld(), op2.getPathNew(), opt);
    BOOST_CHECK_EQUAL(op2.execute(), -1);

    FileOperation::setStrategy(strategy_ptr(new FileMoveOverwrite()));
    BOOST_CHECK_EQUAL(op2.execute(), -1);
    BOOST_CHECK_EQUAL(bf::exists(op2.getPathOld()), false);
    BOOST_CHECK_EQUAL(bf::exists(op2.getPathNew()), true);
}
