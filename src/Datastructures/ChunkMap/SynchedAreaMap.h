#ifndef SYNCHEDAREAMAP_H
#define SYNCHEDAREAMAP_H

#include "ChunkMap.h"

#include <map>
#include <vector>

#include "../../World/SynchedArea.h"
#include "../Coordinate/RegionCoord.h"

using namespace std;

//All SynchedAreas are owned by this class and can be merged/deleted/split here,
//so do not assume that a reference to a SynchedArea will remain valid
//This class is exclusively used by world, so multithreading will not invalidate
//data

class World;

class SynchedAreaMap : protected RegionMap<SynchedArea*>{
protected:
    World* world = 0; //Only needed to pass along as a parameter

    const int stretchMax = 3;//Max dist at which a SynchedArea will be stretched
    //to include new claim (dist in regions)

    vector<SynchedArea*> areas;
    vector<SynchedArea*> areasToDelete;

    //Not to be confused with unclaimRegion from SynchedArea
    void unclaimRegion(JobTicket* j);

    //merge just adds 2 areas together, it doesn't guaruntee they'll be connected
    void merge(SynchedArea*, SynchedArea*); //lhs <-- rhs (lhs is kept)

    friend class World;

public:
    SynchedAreaMap(World* w){
        world = w;
    };
    SynchedAreaMap(){};
    ~SynchedAreaMap();

    void setWorld(World* w){world = w;};

    void maintenance();

    SynchedArea* getSynchedArea(Coordinate<int>); //Might create new one
    SynchedArea* getSynchedArea(Coordinate<double>);

    void extend(SynchedArea*, RegionCoord);

};

#endif
