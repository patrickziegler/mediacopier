#include <gtest/gtest.h>

static double timesTwo(double x)
{
    return x * 2;
}

TEST(testTimesTwo2, integerTests2)
{
    EXPECT_EQ(0, timesTwo(0));
    EXPECT_EQ(2, timesTwo(1));
    EXPECT_EQ(246, timesTwo(123));
}
