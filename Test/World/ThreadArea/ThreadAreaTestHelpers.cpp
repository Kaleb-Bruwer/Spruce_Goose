#include "ThreadAreaTestHelpers.h"

void tAreaClaimChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords){
    for(ChunkCoord c : coords){
        tArea.claims.setVal(c, true);
    }
}

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

void verifyChunks(const ThreadArea &tArea, std::vector<ChunkCoord> coords,
        std::vector<Chunk*> chunks){
    ASSERT_TRUE(coords.size() == chunks.size()) << "Invalid validation";

    for(int i=0; i<coords.size(); i++){
        Chunk* c = tArea.chunks.getVal(coords[i]);
        ASSERT_TRUE(c == chunks[i]) << "Chunk " << coords[i] << " mismatched";
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

namespace ns_ta{

    SynchedArea_mock::SynchedArea_mock(World* w, bool r) : SynchedArea(w, false){
        response = r;
    }

    bool SynchedArea_mock::hasRegion(RegionCoord r){
        return response;
    }

}
