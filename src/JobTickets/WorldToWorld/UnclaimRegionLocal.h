#ifndef UNCLAIMREGIONLOCAL_H
#define UNCLAIMREGIONLOCAL_H

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/RegionCoord.h"

using namespace std;

//Sent from SynchedArea to self to ensure Region is only deleted after
//it received its last chunks from ThreadArea

class UnclaimRegionLocal : public JobTicket{
public:
    UnclaimRegionLocal() : JobTicket(UNCLAIMREGIONLOCAL){};
    RegionCoord r;
};

#endif
