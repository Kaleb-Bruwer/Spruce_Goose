#ifndef EXTERNALPLAYER_H
#define EXTERNALPLAYER_H

#include "../JobTicket.h"

using namespace std;

class PlayerEntity;

class ExternalPlayer : public JobTicket{
public:
    ExternalPlayer() : JobTicket(EXTERNALPLAYER){};

    PlayerEntity* player;
};

#endif
