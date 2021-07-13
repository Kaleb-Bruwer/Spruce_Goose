#pragma once

#include "Cluster.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"

// Generate player: This represents a player on a generation server

class GenPlayer{
private:
    Cluster activeCluster;
    // Used to track progress on activeCluster
    // Only counts desired chunks, not all that are included
    int outstanding = 0;

    // CONNECTION INFO


    // BUFFER
    ChunkMap<Chunk*> uncollected;

    sendChunk(Chunk* c, SynchedArea* s);

public:
    bool onStandby(){
        return outstanding == 0;
    };

    void setCluster(Cluster c);

    bool addChunk(ChunkCoord coord, SynchedArea* dest);

};
