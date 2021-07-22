#ifndef ENTITIESTOPLAYERF_H
#define ENTITIESTOPLAYERF_H

#include "../../General/Functor.h"
#include "../../Entities/PlayerEntity.h"

using namespace std;

//Execute this on specific chunk in EntityStore to send player all
//entities within that chunk, meant to be used when player first joins

//NOTE: Does not send this player to already-loaded players
class EntitiesToPlayerF : public Functor<Entity*>{
protected:
    PlayerEntity* player;
public:
    EntitiesToPlayerF(PlayerEntity* p){
        player = p;
    };

    void operator()(Entity* e){
        e->sendToPlayer(player);
    };
};

#endif
