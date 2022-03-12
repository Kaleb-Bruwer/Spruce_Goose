#ifndef SENDPLAYERPOSTOWORLD_H
#define SENDPLAYERPOSTOWORLD_H

#include "../JobTicket.h"

#include <limits>

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class SendPlayerPosToWorld : public JobTicket{
public:
    int eid;
    bool posIncluded = false;
    Coordinate<double> pos;
    double pitch = numeric_limits<double>::infinity(); //infinity = not included
    double yaw = numeric_limits<double>::infinity();

    unsigned char onGround = 0; //0: not included, 1=false, 3 = true

    SendPlayerPosToWorld() : JobTicket(SENDPLAYERPOSTOWORLD){};
    SendPlayerPosToWorld(Coordinate<double> p, double pi, double y) : JobTicket(SENDPLAYERPOSTOWORLD){
        pos = p;
        pitch = pi;
        yaw = y;
    }
};

#endif
