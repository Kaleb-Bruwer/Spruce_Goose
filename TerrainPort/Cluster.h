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

class Cluster{
protected:
    // renderDistance of terrain-generating servers
    int renderDistance;
    vector<ClusterVal> values;

public:

    // Used to quickly exlude far coordinates, includes renderDistance buffer
    ChunkCoord lowBound;
    ChunkCoord highBound;

    Cluster(int renderDist){
        renderDistance = renderDist;
    };

    bool inBoundingBox(ChunkCoord c){
        if(c.x >= lowBound.x && c.x <= highBound.x &&
                c.z >= lowBound.z && c.z <= highBound.z){
            return true;
        }
        return false;
    };

    bool fitsHere(ChunkCoord c);
    void add(ChunkCoord c, SynchedArea* dest);

    // moves everything from rhs into this
    void merge(Cluster &rhs);
};
