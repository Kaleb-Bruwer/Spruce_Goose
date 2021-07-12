cmake_minimum_required(VERSION 3.10)

#C++ standard
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

#Project name
project(Widecraft)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(zlib)

add_executable(Widecraft
    main.cpp
    TestFunctions.h
    TestFunctions.cpp
    Server.h
    Server.cpp

    ./General/Functor.h
    ./General/FunctorAdapter.h
    ./General/LevelData.h
    ./General/LevelData.cpp
    ./General/BlockStrengths.h
    ./General/BlockStrengths.cpp
    ./General/ToolTable.h
    ./General/ToolTable.cpp
    ./General/ToolSpeedTable.h
    ./General/ToolSpeedTable.cpp
    ./General/StackSizeTable.h
    ./General/StackSizeTable.cpp
    ./General/GameSettings.h
    ./General/GameSettings.cpp

    ./Inventory/InventoryControl.h
    ./Inventory/InventoryControl.cpp
    ./Inventory/Inventory2.h
    ./Inventory/Inventory2.cpp
    ./Inventory/Slot.h
    ./Inventory/Slot.cpp
    ./Inventory/EntityMetadata.h
    ./Inventory/EntityMetadata.cpp

    ./Inventory/Crafting/Crafting.h
    ./Inventory/Crafting/Crafting.cpp
    ./Inventory/Crafting/Recipe.h
    ./Inventory/Crafting/Recipe.cpp
    ./Inventory/Crafting/ShapedRecipe.h
    ./Inventory/Crafting/ShapedRecipe.cpp

    ./Inventory/BlockData/BlockData.h
    ./Inventory/BlockData/BlockData.cpp
    ./Inventory/BlockData/BlockDataS.h
    ./Inventory/BlockData/BlockDataS.cpp
    ./Inventory/BlockData/Chest.h
    ./Inventory/BlockData/Chest.cpp
    ./Inventory/BlockData/CraftingTable.h
    ./Inventory/BlockData/CraftingTable.cpp

    ./Protocol/Protocol.h
    ./Protocol/Protocol.cpp
    ./Protocol/ConnectionHandler.h
    ./Protocol/ConnectionHandler.cpp
    ./Protocol/PlayerConnection.h
    ./Protocol/PlayerConnection.cpp
    ./Protocol/PlayerConnection1_7.h
    ./Protocol/PlayerConnection1_7.cpp
    ./Protocol/ProtocolHelpers.h
    ./Protocol/ProtocolHelpers.cpp
    ./Protocol/PacketWriter.h
    ./Protocol/PacketWriter.cpp
    ./Protocol/AdvancedWriter.h
    ./Protocol/AdvancedWriter.cpp
    ./Protocol/ChunkWriter.h
    ./Protocol/ChunkWriter.cpp
    ./Protocol/BlockChangeWriter.h
    ./Protocol/BlockChangeWriter.cpp
    ./Protocol/PacketWriterOld.h
    ./Protocol/PacketWriterOld.cpp
    ./Protocol/PacketWriterOldMOTD.h
    ./Protocol/PacketWriterOldMOTD.cpp
    ./Protocol/PacketReader.h
    ./Protocol/PacketReader.cpp
    ./Protocol/GetUUID.h
    ./Protocol/GetUUID.cpp

    ./Protocol/PacketHandlers/PacketHandlerRead.h
    ./Protocol/PacketHandlers/PacketHandlerWrite.h
    ./Protocol/PacketHandlers/PacFullChunk.h
    ./Protocol/PacketHandlers/PacFullChunk.cpp
    ./Protocol/PacketHandlers/PacFullChunk1_16_3.h
    ./Protocol/PacketHandlers/PacFullChunk1_16_3.cpp
    ./Protocol/PacketHandlers/PacInventoryS13.h
    ./Protocol/PacketHandlers/PacLoginStartC0.h
    ./Protocol/PacketHandlers/PacPositionAndLookS34.h
    ./Protocol/PacketHandlers/PacSpawnPositionS42.h
    ./Protocol/PacketHandlers/PacketHandlerRead.h
    ./Protocol/PacketHandlers/PacketHandlerRead.cpp

    ./Generator/WorldGenerator.h
    ./Generator/WorldGenerator.cpp
    ./Generator/GeneratorThread.h
    ./Generator/GeneratorThread.cpp
    ./Generator/SuperflatGenerator.h
    ./Generator/SuperflatGenerator.cpp
    ./Generator/TerrainGen1.h
    ./Generator/TerrainGen1.cpp
    ./Generator/noise/PerlinNoise.h
    ./Generator/noise/PerlinNoise.cpp

    ./World/World.h
    ./World/World.cpp
    ./World/Region.h
    ./World/Region.cpp
    ./World/ThreadArea.h
    ./World/ThreadArea.cpp
    ./World/SynchedArea.h
    ./World/SynchedArea.cpp
    ./World/EIDAllocator.h
    ./World/EIDAllocator.cpp
    ./World/EntityStore.h
    ./World/EntityStore.cpp

    ./World/Chunk/Block.h
    ./World/Chunk/Block.cpp
    ./World/Chunk/ChunkSection.h
    ./World/Chunk/ChunkSection.cpp
    ./World/Chunk/Chunk.h
    ./World/Chunk/Chunk.cpp
    ./World/Chunk/CompressedChunk.h
    ./World/Chunk/CompressedChunk.cpp
    ./World/Chunk/ChunkChangeTracker.h
    ./World/Chunk/ChunkChangeTracker.cpp
    ./World/Chunk/CompChunkPacket.h
    ./World/Chunk/CompChunkPacket.cpp
    ./World/Chunk/PlayerToken.h
    ./World/Chunk/PlayerToken.cpp
    ./World/Chunk/BlockChanges.h
    ./World/Chunk/BlockChanges.cpp

    ./World/Functors/BlockUpdatesF.h
    ./World/Functors/ChunkCheckBreaksF.h
    ./World/Functors/PlayerCheckBreaksF.h
    ./World/Functors/PlayerCheckBreaksF.cpp
    ./World/Functors/ItemPickupF.h
    ./World/Functors/ItemPickupF.cpp
    ./World/Functors/ItemUpdateF.h
    ./World/Functors/ItemUpdateF.cpp
    ./World/Functors/PlayerPickupF.h
    ./World/Functors/PlayerPickupF.cpp
    ./World/Functors/PlayerWindowUpdatesF.h
    ./World/Functors/PlayerWindowUpdatesF.cpp
    ./World/Functors/AddChunksToPlayerF.h
    ./World/Functors/AddChunksToPlayerF.cpp
    ./World/Functors/MovEntitiesToSplitF.h
    ./World/Functors/MovEntitiesToSplitF.cpp

    ./Entities/Entity.h
    ./Entities/Entity.cpp
    ./Entities/Item.h
    ./Entities/Item.cpp
    ./Entities/PlayerEntity.h
    ./Entities/PlayerEntity.cpp

    ./TerrainPort/TerrainPort.h
    ./TerrainPort/TerrainPort.cpp
    ./TerrainPort/ClusterGroup.h
    ./TerrainPort/ClusterGroup.cpp
    ./TerrainPort/Cluster.h
    ./TerrainPort/Cluster.cpp
    ./TerrainPort/RequestedCluster.h
    ./TerrainPort/RequestedCluster.cpp

    ./JobTickets/JobTicket.h
    ./JobTickets/JobTicket.cpp
    ./JobTickets/SendPacket.h
    ./JobTickets/SendPacket.cpp

    ./JobTickets/WorldToProtocol/SendPlayerLogin.h
    ./JobTickets/WorldToProtocol/SendPlayerLogin.cpp
    ./JobTickets/WorldToProtocol/ChatToProtocol.h
    ./JobTickets/WorldToProtocol/SendItem.h
    ./JobTickets/WorldToProtocol/SendWindowItem.h
    ./JobTickets/WorldToProtocol/SendPlayerPos.h
    ./JobTickets/WorldToProtocol/SendThreadArea.h
    ./JobTickets/WorldToProtocol/EntityMove.h

    ./JobTickets/ProtocolToWorld/SendPlayerPosToWorld.h
    ./JobTickets/ProtocolToWorld/ChatToWorld.h
    ./JobTickets/ProtocolToWorld/ClientSlotChange.h
    ./JobTickets/ProtocolToWorld/PlayerBlockPlace.h
    ./JobTickets/ProtocolToWorld/PlayerDiggingJob.h

    ./Datastructures/Varint.h
    ./Datastructures/Varint.cpp
    ./Datastructures/BlockingQueue.h
    ./Datastructures/BlockingQueueCounting.h

    ./Datastructures/TieredGrid/TieredGrid.h
    ./Datastructures/TieredGrid/TieredGrid.cpp
    ./Datastructures/TieredGrid/TilePos.h
    ./Datastructures/TieredGrid/TilePos.cpp
    ./Datastructures/TieredGrid/TieredTile.h
    ./Datastructures/TieredGrid/TieredTile.cpp

    ./Datastructures/Coordinate/Coordinate.h
    ./Datastructures/Coordinate/Coordinate.cpp
    ./Datastructures/Coordinate/ChunkCoord.h
    ./Datastructures/Coordinate/ChunkCoord.cpp
    ./Datastructures/Coordinate/RegionCoord.h
    ./Datastructures/Coordinate/RegionCoord.cpp

    ./Datastructures/ChunkMap/ChunkMap.h
    ./Datastructures/ChunkMap/ChunkMap.cpp
    ./Datastructures/ChunkMap/RegionMap.h
    ./Datastructures/ChunkMap/RegionMap.cpp
    ./Datastructures/ChunkMap/CoordAreaPair.h
    ./Datastructures/ChunkMap/CoordAreaPair.cpp
    ./Datastructures/ChunkMap/SynchedAreaMap.h
    ./Datastructures/ChunkMap/SynchedAreaMap.cpp

    ./Datastructures/NBT/NBT.h
    ./Datastructures/NBT/NBT.cpp
    ./Datastructures/NBT/NBT_Primitive.h
    ./Datastructures/NBT/NBT_Primitive.cpp
    ./Datastructures/NBT/NBT_Array.h
    ./Datastructures/NBT/NBT_Array.cpp
    ./Datastructures/NBT/Tag_Byte.h
    ./Datastructures/NBT/Tag_Byte.cpp
    ./Datastructures/NBT/Tag_Short.h
    ./Datastructures/NBT/Tag_Short.cpp
    ./Datastructures/NBT/Tag_Int.h
    ./Datastructures/NBT/Tag_Int.cpp
    ./Datastructures/NBT/Tag_Long.h
    ./Datastructures/NBT/Tag_Long.cpp
    ./Datastructures/NBT/Tag_Long.h
    ./Datastructures/NBT/Tag_Long.cpp
    ./Datastructures/NBT/Tag_Float.h
    ./Datastructures/NBT/Tag_Float.cpp
    ./Datastructures/NBT/Tag_Double.h
    ./Datastructures/NBT/Tag_Double.cpp
    ./Datastructures/NBT/Tag_Byte_Array.h
    ./Datastructures/NBT/Tag_Byte_Array.cpp
    ./Datastructures/NBT/Tag_String.h
    ./Datastructures/NBT/Tag_String.cpp
    ./Datastructures/NBT/Tag_List.h
    ./Datastructures/NBT/Tag_List.cpp
    ./Datastructures/NBT/Tag_Compound.h
    ./Datastructures/NBT/Tag_Compound.cpp
    ./Datastructures/NBT/Tag_Int_Array.h
    ./Datastructures/NBT/Tag_Int_Array.cpp
    ./Datastructures/NBT/Tag_Long_Array.h
    ./Datastructures/NBT/Tag_Long_Array.cpp

    ./Datastructures/Octree/Octree.h
    ./Datastructures/Octree/Octree.cpp
    ./Datastructures/Octree/OctNode.h
    ./Datastructures/Octree/OctNode.cpp
    ./Datastructures/Octree/OctInternal.h
    ./Datastructures/Octree/OctInternal.cpp
    ./Datastructures/Octree/OctLeaf.h
    ./Datastructures/Octree/OctLeaf.cpp
    ./Datastructures/Octree/OctRoot.h
    ./Datastructures/Octree/OctRoot.cpp
    ./Datastructures/Octree/Positional.h
    )

target_link_libraries(Widecraft zlib)
target_link_libraries(Widecraft curl)
target_link_libraries(Widecraft Threads::Threads)
