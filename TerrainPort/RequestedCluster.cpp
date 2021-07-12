#include "RequestedCluster.h"

using namespace std;

bool RequestedCluster::contains(ChunkCoord coord){
    if(!inBoundingBox(coord))
        return false;

    for(ClusterVal &v : values){
        if(v.coord == coord)
            return true;
    }
    return false;
}

void RequestedCluster::setDest(ChunkCoord coord, SynchedArea* dest){
    for(ClusterVal &v : values){
        if(v.coord == coord){
            v.dest = dest;
            return;
        }
    }
}
