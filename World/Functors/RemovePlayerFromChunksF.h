#ifndef REMOVEPLAYERFROMCHUNKSF_H
#define REMOVEPLAYERFROMCHUNKSF_H

#include "../../General/Functor.h"
#include "../Chunk/Chunk.h"
#include "../../Entities/PlayerEntity.h"

using namespace std;

class RemovePlayerFromChunksF : public Functor<Chunk*>{
protected:
    PlayerEntity* player;
public:
    RemovePlayerFromChunksF(PlayerEntity* p){
        player = p;
    };

    void operator()(Chunk* c){
        if(player->hasChunk(c->getChunkCoord())){
            c->removePlayer(player);
            player->removeChunk(c->getChunkCoord());
        }
    };
};

#endif
