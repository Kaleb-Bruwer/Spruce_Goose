#include <gtest/gtest.h>

#include "./Inventory/Inventory2Test.cpp"
#include "./Inventory/CraftingTableTest.cpp"
#include "./Inventory/CraftingTest.cpp"
#include "./Inventory/CraftingFrameTest.cpp"
#include "./Inventory/BaseChestTest.cpp"
#include "./Inventory/FurnaceTest.cpp"

#include "./Protocol/PacketReaderTest.cpp"
#include "./Datastructures/NBTReadTest.cpp"

#include "./General/FuelTableTest.cpp"

#include "./Datastructures/Octree/OctLeafTest.cpp"
#include "./Datastructures/Octree/OctInternalTest.cpp"
#include "./Datastructures/Octree/OctRootTest.cpp"
#include "./Datastructures/Octree/OctreeTest.cpp"

#include "./World/EntityStore/EntityStoreAddTest.cpp"
#include "./World/EntityStore/EntityStoreOutsideTest.cpp"
#include "./World/EntityStore/EntityStoreRemoveTest.cpp"
#include "./World/EntityStore/EntityStoreGetTest.cpp"
#include "./World/EntityStore/EntityStoreRemoveFunctorTest.cpp"

#include "./World/ThreadAreaCallbacksTest.cpp"
#include "./World/Chunk/ChunkChangeTrackerTest.cpp"
#include "./World/Chunk/ChunkSetBlockTest.cpp"
#include "./World/Chunk/ChunkBreakBlockTest.cpp"
#include "./World/Chunk/ChunkStartBreakTest.cpp"
#include "./World/Chunk/ChunkCheckBreaksTest.cpp"


int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
