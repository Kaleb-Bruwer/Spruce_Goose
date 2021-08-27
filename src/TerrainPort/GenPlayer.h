#pragma once

#include "Cluster.h"
#include "GenPlayerConnection.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"
#include "../JobTickets/WorldToGenerator/GenChunkReq2.h"

#include <vector>

// Generate player: This represents a player on a generation server

class MapChunkBulkReader;

class GenPlayer{
private:
    GenChunkReq2* activeJob = 0;
    chrono::high_resolution_clock::time_point jobAge;
    int neededChunks = 0;

    // CONNECTION INFO
    GenPlayerConnection connection;

    void sendChunk(Chunk* c, SynchedArea* s);

public:
    // Set in TerrainPort's constructor
    inline static int renderDistance;

    bool onStandby(){
        auto now = chrono::high_resolution_clock::now();
        int age = chrono::duration_cast<chrono::seconds>(now - jobAge).count();
        if(age > 3)
            return true;
        return neededChunks == 0;
    };

    bool canFitNewCenter(Coordinate<int> pos);

    GenChunkReq2* setJob(GenChunkReq2* job);
    // Moves position and still remembers current job's chunks
    void shiftJob(GenChunkReq2* job);

    //Opens connection to server, returns socket handle
    int activate(std::string username);

    // Reads data from connection and handles it
    // Returns chunks that it doesn't have a destination for
    vector<Chunk*> readMessage();

};
