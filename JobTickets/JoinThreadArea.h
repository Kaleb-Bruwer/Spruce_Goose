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

    JoinThreadArea(PlayerConnection* p){
        player = p;
    };
    JoinThreadArea(PlayerConnection* p, Coordinate<double> position){
        player = p;
        pos = position;
    };

    string getType(){
        return "JoinThreadArea";
    };
};

#endif
