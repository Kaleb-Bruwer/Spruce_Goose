#include "Cluster.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

bool Cluster::fitsHere(ChunkCoord c){
    // Calculate min distance to any coordinate within bounds
    // return true the moment something close enough is found

    for(ClusterVal &v: values){
        ChunkCoord &pos2 = v.coord;
        int distance = min(abs(c.x - pos2.x), abs(c.z - pos2.z));
        if(distance < renderDistance)
            return true;
    }
    return false;
}
