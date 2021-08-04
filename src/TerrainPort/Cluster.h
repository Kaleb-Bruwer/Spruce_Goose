#pragma once

#include <chrono>
#include <vector>

#include "../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

class SynchedArea;

struct ClusterVal{
    ChunkCoord coord;
    SynchedArea* dest = 0;

    chrono::high_resolution_clock::time_point arrive;

    bool wasSent = false;
    chrono::high_resolution_clock::time_point sent;
};

class GenPlayer;

class Cluster{
protected:
    // renderDistance of terrain-generating servers
    ChunkCoord center;

    chrono::high_resolution_clock::time_point oldestArrival;
    vector<ClusterVal> values;
    static const int oldAge = 100; //ms a chunk must wait until it is guarunteed to be sent

    struct{
        short int negX = -1;
        short int posX = -1;
        short int negZ = -1;
        short int posZ = -1;
    } breathingRoom;

    ChunkCoord getNeededCenterMov(ChunkCoord c);

    friend class GenPlayer;
public:
    // Set in TerrainPort's constructor
    inline static int renderDistance;


    bool fitsHere(ChunkCoord c){
        // Calculate min distance to any coordinate within bounds
        // return true the moment something close enough is found
        int distance = max(abs(center.x - c.x), abs(center.z - c.z));
        return(distance <= renderDistance);
    };


    //Returns true if center can be moved in order to fit given coord
    // 1: fits without stretch
    // 2: fits, but must move center
    int fitsIfStretch(ChunkCoord c);

    bool add(ChunkCoord c, SynchedArea* dest);
    SynchedArea* getDest(ChunkCoord c);
    void remove(ChunkCoord c);

    // moves everything from rhs into this
    void merge(Cluster &rhs);

    // true if a chunk arrived long enough ago
    bool isOld();
    // Cluster is ready to be sent (combination of age and % filled)
    bool canSend();

    int numChunks(){
        return values.size();
    }
};
