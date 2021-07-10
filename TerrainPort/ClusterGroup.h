#pragma once

#include <vector>

#include "Cluster.h"

using namespace std;

class ClusterGroup{
private:
    vector<Cluster> clusters;
    int renderDistance;

public:
    ClusterGroup(int renderDist){
        renderDistance = renderDist;
    };

    // Adds chunk that must be requested
    void addChunk(ChunkCoord coord);
};
