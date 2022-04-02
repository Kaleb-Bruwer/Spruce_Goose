#include "ThreadAreaTestHelpers.h"

void tAreaSetChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords){
    for(ChunkCoord c : coords){
        // A zero as a dummyVal will cause problems
        int dummyVal = c.x * 1024 + c.z + 69420;
        tArea.claims.setVal(c, true);
        tArea.chunks.setVal(c, (Chunk*) dummyVal);
    }
}

void tAreaUnsetChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords){
    for(ChunkCoord c : coords){
        tArea.chunks.setVal(c, 0);
    }
}

void verifyChunks(const ThreadArea &tArea, std::vector<ChunkCoord> coords){
    for(ChunkCoord c : coords){
        bool claimed = tArea.claims.getVal(c);
        Chunk* chunk = tArea.chunks.getVal(c);

        ASSERT_TRUE(claimed) << "Chunk " << c << " not claimed";
        ASSERT_TRUE(chunk) << "Chunk " << c << " has no value";
    }
}

void verifyChunksNull(const ThreadArea &tArea, std::vector<ChunkCoord> coords){
    for(ChunkCoord c : coords){
        bool claimed = tArea.claims.getVal(c);
        Chunk* chunk = tArea.chunks.getVal(c);

        ASSERT_FALSE(claimed) << "Chunk " << c << " not unclaimed";
        ASSERT_FALSE(chunk) << "Chunk " << c << " not removed";
    }
}
