#include "ClusterGroup.h"

using namespace std;

void ClusterGroup::addChunk(ChunkCoord coord){
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
        clusters.push_back(Cluster(renderDistance, coord));
    }
    else if(validClusters.size() == 1){
        // Add to existing cluster
        clusters[validClusters[0]].add(coord);
    }
    else{
        // Merge clusters
        
    }
}
