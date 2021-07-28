#pragma once

#include "Cluster.h"
#include "GenPlayerConnection.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"

#include <vector>

// Generate player: This represents a player on a generation server

class MapChunkBulkReader;

class GenPlayer{
private:
    Cluster activeCluster;
    // Used to track progress on activeCluster
    // Only counts desired chunks, not all that are included
    int outstanding = 0;

    // CONNECTION INFO
    GenPlayerConnection connection;

    // BUFFER
    ChunkMap<Chunk*> uncollected;


    void sendChunk(Chunk* c, SynchedArea* s);

public:
    bool onStandby(){
        return outstanding == 0;
    };

    void setCluster(Cluster c);

    bool addChunk(ChunkCoord coord, SynchedArea* dest);

    void readMessage();

};
