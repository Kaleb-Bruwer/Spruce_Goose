#ifndef PLAYERBLOCKPLACE_H
#define PLAYERBLOCKPLACE_H

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class PlayerBlockPlace : public JobTicket{
public:
    PlayerBlockPlace(): JobTicket(PLAYERBLOCKPLACE){};

    int eid;
    Coordinate<int> pos;
    int blockID;
    char cursorX;
    char cursorY;
    char cursorZ;
    char direction;
};


#endif
