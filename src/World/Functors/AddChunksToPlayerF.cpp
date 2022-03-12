#include "AddChunksToPlayerF.h"

#include "../Chunk/Chunk.h"
#include "../EntityStore.h"
#include "../../Entities/PlayerEntity.h"
#include "EntitiesToPlayerF.h"

using namespace std;

AddChunksToPlayerF::AddChunksToPlayerF(PlayerEntity* p, EntityStore* e){
    player = p;
    entities = e;
}

void AddChunksToPlayerF::operator()(Chunk* c){
    ChunkCoord cCoord = c->getChunkCoord();
    if(player->needsChunk(cCoord)){
        //Add player to chunk
        c->addPlayer(player);
        player->addChunk(cCoord);

        //Tell player about entities already in chunk
        EntitiesToPlayerF f(player);
        entities->execFuncInChunk(cCoord, f);
    }
}
