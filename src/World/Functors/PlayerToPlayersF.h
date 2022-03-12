#ifndef PLAYERTOPLAYERSF_H
#define PLAYERTOPLAYERSF_H

#include "../../General/Functor.h"
#include "../../Entities/PlayerEntity.h"
#include "../../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

//Sends player (specified in constructor) to all players this is executed on,
//assuming they have the chunk loaded
class PlayerToPlayersF : public Functor<PlayerEntity*>{
protected:
    PlayerEntity* player;
    ChunkCoord cCoord;
public:
    PlayerToPlayersF(PlayerEntity* p){
        player = p;
        cCoord = p->position.getContainingChunk();
    };

    void operator()(PlayerEntity* rhs){
        if(rhs->hasChunk(cCoord))
            player->sendToPlayer(rhs);
    };
};

#endif
