#include "Cluster.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

void Cluster::add(ChunkCoord c, SynchedArea* dest){
    // cout << "Adding " << c << " to cluster\n";
    ClusterVal val;
    val.coord = c;
    val.dest = dest;
    val.arrive = chrono::high_resolution_clock::now();
    if(values.size() == 0){
        center = c;
        oldestArrival = val.arrive;
    }

    values.push_back(val);
}

void Cluster::merge(Cluster &rhs){
    for(ClusterVal a : rhs.values){
        values.push_back(a);
    }
    rhs.values.clear();
}

SynchedArea* Cluster::getDest(ChunkCoord coord){
    if(!fitsHere(coord))
        return 0;

    for(ClusterVal &c : values){
        if(c.coord == coord)
            return c.dest;
    }
    return 0;
}

void Cluster::remove(ChunkCoord c){
    for(int i=0; i!= values.size(); i++){
        if(values[i].coord == c){
            values.erase(values.begin() + i);
            return;
        }
    }
}

bool Cluster::isOld(){
    auto present = chrono::high_resolution_clock::now();
    int age = chrono::duration_cast<chrono::milliseconds>(present - oldestArrival).count();
    return(age >= oldAge);
}

bool Cluster::canSend(){
    if(isOld())
        return true;

    int fullSize = renderDistance * 2 + 1;
    fullSize *= fullSize;

    const float minCutoff = 0.7;

    return ((double) values.size()) / fullSize >= minCutoff;
}
