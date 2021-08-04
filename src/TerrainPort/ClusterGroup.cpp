#include "ClusterGroup.h"

using namespace std;

void ClusterGroup::addChunk(ChunkCoord coord, SynchedArea* dest){
    for(int i=0; i<clusters.size(); i++){
        if(clusters[i].add(coord, dest))
            return;
    }

    Cluster cluster;
    cluster.add(coord, dest);
    clusters.push_back(cluster);
}

vector<Cluster> ClusterGroup::getReadyClusters(){
    vector<Cluster> result;
    for(auto it = clusters.begin(); it != clusters.end();){
        if(it->canSend()){
            result.push_back(*it);
            it = clusters.erase(it);
        }
        else{
            it++;
        }
    }
    return result;
}
