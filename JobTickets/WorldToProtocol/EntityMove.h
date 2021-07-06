#ifndef ENTITYMOVE_H
#define ENTITYMOVE_H

#include "../JobTicket.h"

#include <climits>

using namespace std;

class EntityMove : public JobTicket{
public:
    EntityMove() : JobTicket(ENTITYMOVE){};
    EntityMove(EntityMove &e) : JobTicket(ENTITYMOVE){
        posIncluded = e.posIncluded;
        posRelative = e.posRelative;
        lookIncluded = e.lookIncluded;

        eid = e.eid;
        movement = e.movement;
        yaw = e.yaw;
        pitch = e.pitch;
        if(e.heldItem)
            heldItem = new Slot(e.heldItem);
    };

    ~EntityMove(){
        if(heldItem)
            delete heldItem;
    }

    bool posIncluded = false;
    bool posRelative = true;
    bool lookIncluded = false;

    int eid;
    Coordinate<double> movement;

    //Double or nothing on rotation, don't just give one
    char yaw;
    char pitch;

    Slot* heldItem = 0;
};

#endif
