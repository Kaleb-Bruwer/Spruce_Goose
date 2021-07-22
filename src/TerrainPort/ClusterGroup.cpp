#include "ClusterGroup.h"

using namespace std;

void ClusterGroup::addChunk(ChunkCoord coord, SynchedArea* dest){
    vector<int> validClusters;

    // In future: do more efficient search than checking EVERY bounding box
    // Some kind of tree, maybe
    for(int i=0; i<clusters.size(); i++){
        if(!clusters[i].inBoundingBox(coord))
            continue;
        if(clusters[i].fitsHere(coord))
            validClusters.push_back(i);
    }

    if(validClusters.size() == 0){
        // Create new cluster
        Cluster cluster;
        cluster.add(coord, dest);
        clusters.push_back(cluster);
    }
    else if(validClusters.size() == 1){
        // Add to existing cluster
        clusters[validClusters[0]].add(coord, dest);
    }
    else{
        // Merge clusters
        for(int i=1; i<validClusters.size(); i++){
            clusters[validClusters[0]].merge(clusters[validClusters[i]]);
        }
        clusters[validClusters[0]].add(coord, dest);

        for(int i=validClusters.size()-1; i>=0; i--){
            clusters.erase(clusters.begin() + i);
        }
    }
}
