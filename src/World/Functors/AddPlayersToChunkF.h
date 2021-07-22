#ifndef ADDPLAYERSTOCHUNKF_H
#define ADDPLAYERSTOCHUNKF_H

#include "../../General/Functor.h"
#include "../Chunk/Chunk.h"
#include "../../Entities/PlayerEntity.h"

using namespace std;

//Used to add players to a newly-loaded chunk
//Chunk was just loaded and these players are already there, in range of it

class AddPlayersToChunkF : public Functor<PlayerEntity*>{
protected:
    Chunk* chunk;
    ChunkCoord cCoord;
public:
    AddPlayersToChunkF(Chunk* c){
        chunk = c;
        cCoord = chunk->getChunkCoord();
    };

    void operator()(PlayerEntity* p){
        if(p->needsChunk(cCoord)){
            chunk->addPlayer(p);
            p->loadedChunks.setVal(cCoord, true);
        }
    }
};

#endif
