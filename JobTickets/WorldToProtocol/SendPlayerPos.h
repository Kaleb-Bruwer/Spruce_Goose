#ifndef SENDPLAYERPOS_H
#define SENDPLAYERPOS_H

#include "../JobTicket.h"

#include <limits>

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class SendPlayerPos : public JobTicket{
public:
    Coordinate<double> pos;
    double pitch = numeric_limits<double>::infinity(); //infinity = not included
    double yaw = numeric_limits<double>::infinity();

    unsigned char onGround = 0; //0: not included, 1=false, 3 = true

    SendPlayerPos() : JobTicket(SENDPLAYERPOS){};
    SendPlayerPos(Coordinate<double> p, double pi, double y) : JobTicket(SENDPLAYERPOS){
        pos = p;
        pitch = pi;
        yaw = y;
    };
};

#endif
