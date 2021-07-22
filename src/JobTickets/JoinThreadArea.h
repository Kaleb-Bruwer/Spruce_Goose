#ifndef JOINTHREADAREA_H
#define JOINTHREADAREA_H

#include "JobTicket.h"

#include "../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class PlayerConnection;

class JoinThreadArea : public JobTicket{
public:
    PlayerConnection* player;
    Coordinate<double> pos;

    JoinThreadArea(PlayerConnection* p) : JobTicket(JOINTHREADAREA){
        player = p;
    };
    JoinThreadArea(PlayerConnection* p, Coordinate<double> position) : JobTicket(JOINTHREADAREA){
        player = p;
        pos = position;
    };

};

#endif
