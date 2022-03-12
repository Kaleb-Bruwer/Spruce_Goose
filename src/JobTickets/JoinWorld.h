#ifndef JOINWORLD_H
#define JOINWORLD_H

#include "JobTicket.h"

using namespace std;

class JoinWorld : public JobTicket{
public:
    PlayerConnection* player;

    JoinWorld(PlayerConnection* p) : JobTicket(JOINWORLD){
        player = p;
    };
};

#endif
