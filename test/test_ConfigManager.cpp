#define BOOST_TEST_MODULE test_MediaCopy
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "ConfigManager.hpp"

BOOST_AUTO_TEST_SUITE(test_ConfigManager)

BOOST_AUTO_TEST_CASE(parseArgs_all_good)
{
    const int argc = 9;
    const char *argu[argc];

    argu[0] = "script";
    argu[1] = "-o";
    argu[2] = "-s";
    argu[3] = "-f";
    argu[4] = "%Y/%Y-%m/%Y-%m-%d/TEST_%Y%m%d_%H%M%S_%f";
    argu[5] = "-l";
    argu[6] = "out/log.txt";
    argu[7] = "data/src";
    argu[8] = "data/dst";

    char *argv[argc];

    for (int i=0; i<argc; ++i) {
        argv[i] = const_cast<char*>(argu[i]);
    }

    ConfigManager *config = &ConfigManager::instance();

    BOOST_CHECK_EQUAL(config->parseArgs(argc, argv), 0);
    BOOST_CHECK(config->flagOverride);
    BOOST_CHECK(config->flagSimulate);
    BOOST_CHECK_EQUAL(config->pathFormat, "%Y/%Y-%m/%Y-%m-%d/TEST_%Y%m%d_%H%M%S_%f");
    BOOST_CHECK_EQUAL(config->logfile, "out/log.txt");
    BOOST_CHECK_EQUAL(config->dirInput.string(), "data/src");
    BOOST_CHECK_EQUAL(config->dirOutput.string(), "data/dst");
}

BOOST_AUTO_TEST_CASE(parseArgs_combined_options)
{
    const int argc = 5;
    const char *argu[argc];

    argu[0] = "script";
    argu[1] = "-ofs";
    argu[2] = "%Y/%Y-%m/%Y-%m-%d/TEST_%Y%m%d_%H%M%S_%f";
    argu[3] = "data/src";
    argu[4] = "data/dst";

    char *argv[argc];

    for (int i=0; i<argc; ++i) {
        argv[i] = const_cast<char*>(argu[i]);
    }

    ConfigManager *config = &ConfigManager::instance();
    config->parseArgs(argc, argv);

    BOOST_CHECK(config->flagOverride);
    BOOST_CHECK(config->flagSimulate);
    BOOST_CHECK_EQUAL(config->pathFormat, "%Y/%Y-%m/%Y-%m-%d/TEST_%Y%m%d_%H%M%S_%f");
}

BOOST_AUTO_TEST_CASE(parseArgs_exit_on_help)
{
    const int argc = 4;
    const char *argu[argc];

    argu[0] = "script";
    argu[1] = "-h";
    argu[2] = "data/src";
    argu[3] = "data/dst";

    char *argv[argc];

    for (int i=0; i<argc; ++i) {
        argv[i] = const_cast<char*>(argu[i]);
    }

    ConfigManager *config = &ConfigManager::instance();
    BOOST_CHECK_EQUAL(config->parseArgs(argc, argv), 1);
}

BOOST_AUTO_TEST_CASE(parseArgs_missing_positional)
{
    const int argc = 4;
    const char *argu[argc];

    argu[0] = "script";
    argu[1] = "-ofs";
    argu[2] = "%Y/%Y-%m/%Y-%m-%d/TEST_%Y%m%d_%H%M%S_%f";
    argu[3] = "data/src";

    char *argv[argc];

    for (int i=0; i<argc; ++i) {
        argv[i] = const_cast<char*>(argu[i]);
    }

    ConfigManager *config = &ConfigManager::instance();
    BOOST_CHECK_EQUAL(config->parseArgs(argc, argv), 1);
}

BOOST_AUTO_TEST_SUITE_END()
