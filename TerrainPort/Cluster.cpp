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

void Cluster::add(ChunkCoord c, SynchedArea* dest){
    ClusterVal val;
    val.coord = c;
    val.dest = dest;
    val.arrive = chrono::high_resolution_clock::now();
    if(values.size() == 0)
        oldestArrival = val.arrive;

    values.push_back(val);

    // Update bounding box
    if(c.x + renderDistance > highBound.x)
    highBound.x = c.x + renderDistance;
    else if(c.x - renderDistance < lowBound.x)
    lowBound.x = c.x - renderDistance;


    if(c.z + renderDistance > highBound.z)
    highBound.z = c.z + renderDistance;
    else if(c.z - renderDistance < lowBound.z)
    lowBound.z = c.z - renderDistance;
}

void Cluster::merge(Cluster &rhs){
    lowBound.x = min(lowBound.x, rhs.lowBound.x);
    lowBound.z = min(lowBound.z, rhs.lowBound.z);

    highBound.x = max(highBound.x, rhs.highBound.x);
    highBound.z = max(highBound.z, rhs.highBound.z);

    for(ClusterVal a : rhs.values){
        values.push_back(a);
    }
    rhs.values.clear();
}

bool Cluster::isOld(){
    auto present = chrono::high_resolution_clock::now();
    int age = chrono::duration_cast<chrono::milliseconds>(present - oldestArrival).count();
    return(age >= oldAge);
}
