#pragma once

#include <vector>

#include "Cluster.h"

using namespace std;

class ClusterGroup{
private:
    vector<Cluster> clusters;
    int renderDistance;

public:
    ClusterGroup(){};
    ClusterGroup(int renderDist){
        renderDistance = renderDist;
    };
    void setRenderDistance(int r){
        renderDistance = r;
    };

    // Adds chunk that must be requested
    void addChunk(ChunkCoord coord);
};
