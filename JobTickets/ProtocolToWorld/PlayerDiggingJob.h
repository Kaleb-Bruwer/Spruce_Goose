#ifndef PLAYERDIGGINGJOB_H
#define PLAYERDIGGINGJOB_H

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class PlayerDiggingJob : public JobTicket{
public:
    PlayerDiggingJob() : JobTicket(PLAYERDIGGINGJOB){};

    int eid;
    short action;
    Coordinate<int> pos;
    short face;
};


#endif
