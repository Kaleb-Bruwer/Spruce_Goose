#pragma once

#include <vector>

#include "Cluster.h"

using namespace std;

class ClusterGroup{
private:
    vector<Cluster> clusters;

public:
    // Adds chunk that must be requested
    void addChunk(ChunkCoord coord);
};
