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
    // To be used from ClusterGroup, not Cluster (skip excessive func. calls)
    ChunkCoord lowBound;
    ChunkCoord highBound;

    Cluster(int renderDist){
        renderDistance = renderDist;
    };

    bool fitsHere(ChunkCoord c);
};
