#pragma once

#include "../JobTicket.h"

#include <vector>

#include "../../Datastructures/Coordinate/ChunkCoord.h"
#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class SynchedArea;
class ThreadArea;
class GenPlayer;

class GenChunkReq2 : public JobTicket{
private:
    void removeChunk(ChunkCoord c);
    bool contains(ChunkCoord c);


    friend class GenPlayer;

public:
    GenChunkReq2() : JobTicket(GENCHUNKREQ2){};

    Coordinate<int> playerPos;

    vector<ChunkCoord> chunks;

    // Used by TerrainInterface as return address for chunks
    SynchedArea* origin = 0;
    ThreadArea* tArea = 0;
};
