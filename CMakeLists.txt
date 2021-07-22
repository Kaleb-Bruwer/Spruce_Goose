cmake_minimum_required(VERSION 3.10)

#C++ standard
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

#Project name
project(Widecraft)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(src/zlib)


#Excludes main.cpp so that this can be used in test _and_ Widecraft
SET (SRCS
    ./src/TestFunctions.h
    ./src/TestFunctions.cpp
    ./src/Server.h
    ./src/Server.cpp

    ./src/General/Functor.h
    ./src/General/FunctorAdapter.h
    ./src/General/LevelData.h
    ./src/General/LevelData.cpp
    ./src/General/BlockStrengths.h
    ./src/General/BlockStrengths.cpp
    ./src/General/ToolTable.h
    ./src/General/ToolTable.cpp
    ./src/General/ToolSpeedTable.h
    ./src/General/ToolSpeedTable.cpp
    ./src/General/StackSizeTable.h
    ./src/General/StackSizeTable.cpp
    ./src/General/GameSettings.h
    ./src/General/GameSettings.cpp

    ./src/Inventory/InventoryControl.h
    ./src/Inventory/InventoryControl.cpp
    ./src/Inventory/Inventory2.h
    ./src/Inventory/Inventory2.cpp
    ./src/Inventory/Slot.h
    ./src/Inventory/Slot.cpp
    ./src/Inventory/EntityMetadata.h
    ./src/Inventory/EntityMetadata.cpp

    ./src/Inventory/Crafting/Crafting.h
    ./src/Inventory/Crafting/Crafting.cpp
    ./src/Inventory/Crafting/Recipe.h
    ./src/Inventory/Crafting/Recipe.cpp
    ./src/Inventory/Crafting/ShapedRecipe.h
    ./src/Inventory/Crafting/ShapedRecipe.cpp

    ./src/Inventory/BlockData/BlockData.h
    ./src/Inventory/BlockData/BlockData.cpp
    ./src/Inventory/BlockData/BlockDataS.h
    ./src/Inventory/BlockData/BlockDataS.cpp
    ./src/Inventory/BlockData/Chest.h
    ./src/Inventory/BlockData/Chest.cpp
    ./src/Inventory/BlockData/CraftingTable.h
    ./src/Inventory/BlockData/CraftingTable.cpp

    ./src/Protocol/Protocol.h
    ./src/Protocol/Protocol.cpp
    ./src/Protocol/ConnectionHandler.h
    ./src/Protocol/ConnectionHandler.cpp
    ./src/Protocol/PlayerConnection.h
    ./src/Protocol/PlayerConnection.cpp
    ./src/Protocol/PlayerConnection1_7.h
    ./src/Protocol/PlayerConnection1_7.cpp
    ./src/Protocol/ProtocolHelpers.h
    ./src/Protocol/ProtocolHelpers.cpp
    ./src/Protocol/PacketWriter.h
    ./src/Protocol/PacketWriter.cpp
    ./src/Protocol/AdvancedWriter.h
    ./src/Protocol/AdvancedWriter.cpp
    ./src/Protocol/ChunkWriter.h
    ./src/Protocol/ChunkWriter.cpp
    ./src/Protocol/BlockChangeWriter.h
    ./src/Protocol/BlockChangeWriter.cpp
    ./src/Protocol/PacketWriterOld.h
    ./src/Protocol/PacketWriterOld.cpp
    ./src/Protocol/PacketWriterOldMOTD.h
    ./src/Protocol/PacketWriterOldMOTD.cpp
    ./src/Protocol/PacketReader.h
    ./src/Protocol/PacketReader.cpp
    ./src/Protocol/GetUUID.h
    ./src/Protocol/GetUUID.cpp

    ./src/Protocol/PacketHandlers/PacketHandlerRead.h
    ./src/Protocol/PacketHandlers/PacketHandlerWrite.h
    ./src/Protocol/PacketHandlers/PacFullChunk.h
    ./src/Protocol/PacketHandlers/PacFullChunk.cpp
    ./src/Protocol/PacketHandlers/PacFullChunk1_16_3.h
    ./src/Protocol/PacketHandlers/PacFullChunk1_16_3.cpp
    ./src/Protocol/PacketHandlers/PacInventoryS13.h
    ./src/Protocol/PacketHandlers/PacLoginStartC0.h
    ./src/Protocol/PacketHandlers/PacPositionAndLookS34.h
    ./src/Protocol/PacketHandlers/PacSpawnPositionS42.h
    ./src/Protocol/PacketHandlers/PacketHandlerRead.h
    ./src/Protocol/PacketHandlers/PacketHandlerRead.cpp

    ./src/Generator/WorldGenerator.h
    ./src/Generator/WorldGenerator.cpp
    ./src/Generator/GeneratorThread.h
    ./src/Generator/GeneratorThread.cpp
    ./src/Generator/SuperflatGenerator.h
    ./src/Generator/SuperflatGenerator.cpp
    ./src/Generator/TerrainGen1.h
    ./src/Generator/TerrainGen1.cpp
    ./src/Generator/noise/PerlinNoise.h
    ./src/Generator/noise/PerlinNoise.cpp

    ./src/World/World.h
    ./src/World/World.cpp
    ./src/World/Region.h
    ./src/World/Region.cpp
    ./src/World/ThreadArea.h
    ./src/World/ThreadArea.cpp
    ./src/World/SynchedArea.h
    ./src/World/SynchedArea.cpp
    ./src/World/EIDAllocator.h
    ./src/World/EIDAllocator.cpp
    ./src/World/EntityStore.h
    ./src/World/EntityStore.cpp

    ./src/World/Chunk/Block.h
    ./src/World/Chunk/Block.cpp
    ./src/World/Chunk/ChunkSection.h
    ./src/World/Chunk/ChunkSection.cpp
    ./src/World/Chunk/Chunk.h
    ./src/World/Chunk/Chunk.cpp
    ./src/World/Chunk/CompressedChunk.h
    ./src/World/Chunk/CompressedChunk.cpp
    ./src/World/Chunk/ChunkChangeTracker.h
    ./src/World/Chunk/ChunkChangeTracker.cpp
    ./src/World/Chunk/CompChunkPacket.h
    ./src/World/Chunk/CompChunkPacket.cpp
    ./src/World/Chunk/PlayerToken.h
    ./src/World/Chunk/PlayerToken.cpp
    ./src/World/Chunk/BlockChanges.h
    ./src/World/Chunk/BlockChanges.cpp

    ./src/World/Functors/BlockUpdatesF.h
    ./src/World/Functors/ChunkCheckBreaksF.h
    ./src/World/Functors/PlayerCheckBreaksF.h
    ./src/World/Functors/PlayerCheckBreaksF.cpp
    ./src/World/Functors/ItemPickupF.h
    ./src/World/Functors/ItemPickupF.cpp
    ./src/World/Functors/ItemUpdateF.h
    ./src/World/Functors/ItemUpdateF.cpp
    ./src/World/Functors/PlayerPickupF.h
    ./src/World/Functors/PlayerPickupF.cpp
    ./src/World/Functors/PlayerWindowUpdatesF.h
    ./src/World/Functors/PlayerWindowUpdatesF.cpp
    ./src/World/Functors/AddChunksToPlayerF.h
    ./src/World/Functors/AddChunksToPlayerF.cpp
    ./src/World/Functors/MovEntitiesToSplitF.h
    ./src/World/Functors/MovEntitiesToSplitF.cpp

    ./src/Entities/Entity.h
    ./src/Entities/Entity.cpp
    ./src/Entities/Item.h
    ./src/Entities/Item.cpp
    ./src/Entities/PlayerEntity.h
    ./src/Entities/PlayerEntity.cpp

    ./src/TerrainPort/TerrainPort.h
    ./src/TerrainPort/TerrainPort.cpp
    ./src/TerrainPort/ClusterGroup.h
    ./src/TerrainPort/ClusterGroup.cpp
    ./src/TerrainPort/Cluster.h
    ./src/TerrainPort/Cluster.cpp
    ./src/TerrainPort/GenPlayer.h
    ./src/TerrainPort/GenPlayer.cpp

    ./src/JobTickets/JobTicket.h
    ./src/JobTickets/JobTicket.cpp
    ./src/JobTickets/SendPacket.h
    ./src/JobTickets/SendPacket.cpp

    ./src/JobTickets/WorldToProtocol/SendPlayerLogin.h
    ./src/JobTickets/WorldToProtocol/SendPlayerLogin.cpp
    ./src/JobTickets/WorldToProtocol/ChatToProtocol.h
    ./src/JobTickets/WorldToProtocol/SendItem.h
    ./src/JobTickets/WorldToProtocol/SendWindowItem.h
    ./src/JobTickets/WorldToProtocol/SendPlayerPos.h
    ./src/JobTickets/WorldToProtocol/SendThreadArea.h
    ./src/JobTickets/WorldToProtocol/EntityMove.h

    ./src/JobTickets/ProtocolToWorld/SendPlayerPosToWorld.h
    ./src/JobTickets/ProtocolToWorld/ChatToWorld.h
    ./src/JobTickets/ProtocolToWorld/ClientSlotChange.h
    ./src/JobTickets/ProtocolToWorld/PlayerBlockPlace.h
    ./src/JobTickets/ProtocolToWorld/PlayerDiggingJob.h

    ./src/Datastructures/Varint.h
    ./src/Datastructures/Varint.cpp
    ./src/Datastructures/BlockingQueue.h
    ./src/Datastructures/BlockingQueueCounting.h

    ./src/Datastructures/TieredGrid/TieredGrid.h
    ./src/Datastructures/TieredGrid/TieredGrid.cpp
    ./src/Datastructures/TieredGrid/TilePos.h
    ./src/Datastructures/TieredGrid/TilePos.cpp
    ./src/Datastructures/TieredGrid/TieredTile.h
    ./src/Datastructures/TieredGrid/TieredTile.cpp

    ./src/Datastructures/Coordinate/Coordinate.h
    ./src/Datastructures/Coordinate/Coordinate.cpp
    ./src/Datastructures/Coordinate/ChunkCoord.h
    ./src/Datastructures/Coordinate/ChunkCoord.cpp
    ./src/Datastructures/Coordinate/RegionCoord.h
    ./src/Datastructures/Coordinate/RegionCoord.cpp

    ./src/Datastructures/ChunkMap/ChunkMap.h
    ./src/Datastructures/ChunkMap/ChunkMap.cpp
    ./src/Datastructures/ChunkMap/RegionMap.h
    ./src/Datastructures/ChunkMap/RegionMap.cpp
    ./src/Datastructures/ChunkMap/CoordAreaPair.h
    ./src/Datastructures/ChunkMap/CoordAreaPair.cpp
    ./src/Datastructures/ChunkMap/SynchedAreaMap.h
    ./src/Datastructures/ChunkMap/SynchedAreaMap.cpp

    ./src/Datastructures/NBT/NBT.h
    ./src/Datastructures/NBT/NBT.cpp
    ./src/Datastructures/NBT/NBT_Primitive.h
    ./src/Datastructures/NBT/NBT_Primitive.cpp
    ./src/Datastructures/NBT/NBT_Array.h
    ./src/Datastructures/NBT/NBT_Array.cpp
    ./src/Datastructures/NBT/Tag_Byte.h
    ./src/Datastructures/NBT/Tag_Byte.cpp
    ./src/Datastructures/NBT/Tag_Short.h
    ./src/Datastructures/NBT/Tag_Short.cpp
    ./src/Datastructures/NBT/Tag_Int.h
    ./src/Datastructures/NBT/Tag_Int.cpp
    ./src/Datastructures/NBT/Tag_Long.h
    ./src/Datastructures/NBT/Tag_Long.cpp
    ./src/Datastructures/NBT/Tag_Long.h
    ./src/Datastructures/NBT/Tag_Long.cpp
    ./src/Datastructures/NBT/Tag_Float.h
    ./src/Datastructures/NBT/Tag_Float.cpp
    ./src/Datastructures/NBT/Tag_Double.h
    ./src/Datastructures/NBT/Tag_Double.cpp
    ./src/Datastructures/NBT/Tag_Byte_Array.h
    ./src/Datastructures/NBT/Tag_Byte_Array.cpp
    ./src/Datastructures/NBT/Tag_String.h
    ./src/Datastructures/NBT/Tag_String.cpp
    ./src/Datastructures/NBT/Tag_List.h
    ./src/Datastructures/NBT/Tag_List.cpp
    ./src/Datastructures/NBT/Tag_Compound.h
    ./src/Datastructures/NBT/Tag_Compound.cpp
    ./src/Datastructures/NBT/Tag_Int_Array.h
    ./src/Datastructures/NBT/Tag_Int_Array.cpp
    ./src/Datastructures/NBT/Tag_Long_Array.h
    ./src/Datastructures/NBT/Tag_Long_Array.cpp

    ./src/Datastructures/Octree/Octree.h
    ./src/Datastructures/Octree/Octree.cpp
    ./src/Datastructures/Octree/OctNode.h
    ./src/Datastructures/Octree/OctNode.cpp
    ./src/Datastructures/Octree/OctInternal.h
    ./src/Datastructures/Octree/OctInternal.cpp
    ./src/Datastructures/Octree/OctLeaf.h
    ./src/Datastructures/Octree/OctLeaf.cpp
    ./src/Datastructures/Octree/OctRoot.h
    ./src/Datastructures/Octree/OctRoot.cpp
    ./src/Datastructures/Octree/Positional.h
    )

add_executable(Widecraft
    ./src/main.cpp
    ${SRCS}
    )

add_executable(test
    ./Test/testMain.cpp

    ${SRCS}
)


target_link_libraries(test ${GTEST_LIBRARIES} pthread)
target_link_libraries(test zlib)
target_link_libraries(test curl)

target_link_libraries(Widecraft zlib)
target_link_libraries(Widecraft curl)
target_link_libraries(Widecraft Threads::Threads)
