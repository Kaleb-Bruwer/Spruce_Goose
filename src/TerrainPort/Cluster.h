#pragma once

#include <chrono>
#include <vector>

#include "../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

class SynchedArea;

struct ClusterVal{
    ChunkCoord coord;
    SynchedArea* dest = 0;

    chrono::high_resolution_clock::time_point arrive;

    bool wasSent = false;
    chrono::high_resolution_clock::time_point sent;
};

class GenPlayer;

class Cluster{
protected:
    // renderDistance of terrain-generating servers
    ChunkCoord center;

    chrono::high_resolution_clock::time_point oldestArrival;
    vector<ClusterVal> values;
    static const int oldAge = 100; //ms a chunk must wait until it is guarunteed to be sent

    friend class GenPlayer;

public:
    // Set in TerrainPort's constructor
    inline static int renderDistance;

    // Used to quickly exlude far coordinates, includes renderDistance buffer
    ChunkCoord lowBound;
    ChunkCoord highBound;

    bool inBoundingBox(ChunkCoord c){
        if(c.x >= lowBound.x && c.x <= highBound.x &&
                c.z >= lowBound.z && c.z <= highBound.z){
            return true;
        }
        return false;
    };

    bool fitsHere(ChunkCoord c);
    void add(ChunkCoord c, SynchedArea* dest);
    SynchedArea* getDest(ChunkCoord c);
    void remove(ChunkCoord c);

    // moves everything from rhs into this
    void merge(Cluster &rhs);

    // true if a chunk arrived long enough ago
    bool isOld();
    // Cluster is ready to be sent (combination of age and % filled)
    bool canSend();
};
