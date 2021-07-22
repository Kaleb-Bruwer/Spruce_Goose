#include <gtest/gtest.h>

#include "./TerrainPort/TerrainPortTest.cpp"
#include "./TerrainPort/GenPlayerConnectionTest.cpp"

int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
