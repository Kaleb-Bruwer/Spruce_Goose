#include <gtest/gtest.h>

TEST(aTest, Positive){
    ASSERT_EQ(0, 0);
}

TEST(bTest, Negative){
    ASSERT_EQ(0x10, 16);
}

int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
