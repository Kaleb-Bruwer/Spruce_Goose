#include <gtest/gtest.h>

#include "testA.cpp"
#include "./TerrainPort/TerrainPortTest.cpp"

TEST(bTest, Negative){
    ASSERT_EQ(0x10, 16);
}

int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
