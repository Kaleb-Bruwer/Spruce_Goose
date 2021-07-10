#pragma once

#include <chrono>
#include <vector>

#include "../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

struct ClusterVal{
    ChunkCoord coord;
    chrono::high_resolution_clock::time_point arrive;

    bool wasSent = false;
    chrono::high_resolution_clock::time_point sent;
};

class Cluster{
private:
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

    Cluster(int renderDist, ChunkCoord c) : Cluster(renderDist){
        add(c);
    };

    bool inBoundingBox(ChunkCoord c){
        if(c.x >= lowBound.x && c.x <= highBound.x &&
                c.z >= lowBound.z && c.z <= highBound.z){
            return true;
        }
        return false;
    };

    bool fitsHere(ChunkCoord c);
    void add(ChunkCoord c);

    // moves everything from rhs into this
    void merge(Cluster &rhs);
};
