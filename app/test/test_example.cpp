#include <gtest/gtest.h>

double timesThree(double x)
{
    return x * 3;
}

TEST(testTimesThree, integerTests)
{
    EXPECT_EQ(0, timesThree(0));
    EXPECT_EQ(3, timesThree(1));
    EXPECT_EQ(369, timesThree(123));
}
