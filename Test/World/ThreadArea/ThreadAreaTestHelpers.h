#pragma once

#include <gtest/gtest.h>
#include <vector>

#include "../../../src/World/ThreadArea.h"

void tAreaClaimChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords);
// Uses fake chunks, don't use this if Chunks actually need to be used
void tAreaSetChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords);
// Prevents a segfault upon ThreadArea destruction
void tAreaUnsetChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords);

// Only a positive check, doesn't check for the lack of other chunks
void verifyChunks(const ThreadArea &tArea, std::vector<ChunkCoord> coords);
void verifyChunksNull(const ThreadArea &tArea, std::vector<ChunkCoord> coords);
void verifyChunks(const ThreadArea &tArea, std::vector<ChunkCoord> coords,
            std::vector<Chunk*> chunks);

namespace ns_ta{
    class SynchedArea_mock : public SynchedArea{
        bool response;
    public:
        SynchedArea_mock(World* w, bool response = true);

        virtual bool hasRegion(RegionCoord r);
    };
}
