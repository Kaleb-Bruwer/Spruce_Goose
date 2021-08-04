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

    void sendChunk(Chunk* c, SynchedArea* s);

public:
    bool onStandby(){
        return (activeCluster.numChunks() == 0) ||(outstanding == 0);
    };

    //Opens connection to server, returns socket handle
    int activate(std::string username);

    void setCluster(Cluster c);

    bool addChunk(ChunkCoord coord, SynchedArea* dest);

    // Reads data from connection and handles it
    // Returns chunks that it doesn't have a destination for
    vector<Chunk*> readMessage();

};
