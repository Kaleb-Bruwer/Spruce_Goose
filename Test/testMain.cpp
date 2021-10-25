#include <gtest/gtest.h>

#include "./Inventory/Inventory2Test.cpp"
#include "./Inventory/CraftingTableTest.cpp"
#include "./Inventory/CraftingTest.cpp"
#include "./Inventory/CraftingFrameTest.cpp"
#include "./Protocol/PacketReaderTest.cpp"
#include "./Datastructures/NBTReadTest.cpp"


int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
