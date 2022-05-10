#include "PlayerCheckBreaksF.h"

#include "../../Entities/Item.h"
#include "../../Entities/PlayerEntity.h"
#include "../Chunk/Chunk.h"
#include "../ThreadArea.h"

using namespace std;

void PlayerCheckBreaksF::operator()(PlayerEntity* p){
    if(!p->isBreaking){
        if(p->breakCooldown > 0)
            p->breakCooldown--;
        else if(p->breakCooldown == 0 && p->doneBreaking){
            p->doneBreaking = false;
            area->startDigging(p, p->breakTarget);
        }
        return;
    }
    else if(p->isBreaking && p->doneBreaking){
        //Player thinks block is broken
        ChunkCoord cCoord = p->breakTarget.getContainingChunk();
        Chunk* c = area->chunks.getVal(cCoord);

        int tLeft = c->breakTicksLeft(p->breakTarget);
        if(tLeft <= 0){
            Block target = c->getBlock(p->breakTarget);

            vector<Item*> drops = c->breakBlock(p->breakTarget, p->inventory.getHeldItem());
            for(int i=0; i<drops.size(); i++){
                area->newItemToPlayers(drops[i]);
                area->entities.addEntity(drops[i]);
            }

            p->isBreaking = false;
            p->doneBreaking = false;
        }
        else if(tLeft == INT_MAX){
            p->isBreaking = false;
            p->doneBreaking = false;
        }
    }
}
