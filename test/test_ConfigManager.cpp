#define BOOST_TEST_MODULE test_MediaCopy
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test1_suite)

BOOST_AUTO_TEST_CASE(Test1)
{
    int i = 4;
    BOOST_WARN(i>6);
}

BOOST_AUTO_TEST_SUITE_END()
