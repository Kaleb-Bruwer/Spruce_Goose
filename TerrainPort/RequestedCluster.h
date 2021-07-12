#pragma once

#include "Cluster.h"
#include "../Datastructures/Coordinate/Coordinate.h"

// Also add info required to make & monitor request here
// Fully populates values with all chunks that it covers

class RequestedCluster : public Cluster{
private:
    Coordinate<int> sentPosition;

public:
    // Whether or not this request covers a specific chunk
    bool contains(ChunkCoord coord);

    // Assumes valid input
    void setDest(ChunkCoord coord, SynchedArea* dest);
};
