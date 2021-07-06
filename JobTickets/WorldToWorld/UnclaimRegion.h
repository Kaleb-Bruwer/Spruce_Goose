#ifndef UNCLAIMREGION_H
#define UNCLAIMREGION_H

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/RegionCoord.h"

using namespace std;

//Sent from SynchedArea to SynchedAreaMap

//Separating this from UnclaimRegionLocal might have been overkill,
//but in general I don't want to reuse JobTickets since that will
//create near-"blind" dependenies

class UnclaimRegion : public JobTicket{
public:
    RegionCoord r;

    string getType(){return "UnclaimRegion";};
};

#endif
