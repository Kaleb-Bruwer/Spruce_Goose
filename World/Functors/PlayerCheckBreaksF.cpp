#include "PlayerCheckBreaksF.h"

#include "../../Entities/Item.h"
#include "../../Entities/PlayerEntity.h"
#include "../Chunk/Chunk.h"
#include "../ThreadArea.h"

using namespace std;

void PlayerCheckBreaksF::operator()(PlayerEntity* p){
    if(!p->isBreaking && p->breakCooldown > 0){
        p->breakCooldown--;

        if(p->breakCooldown == 0 && p->doneBreaking == true){
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
            c->setBlock(p->breakTarget, Block(0));

            Slot dropS = target.getDrop(p->inventory.getHeldItem());
            if(!dropS.isEmpty()){
                Item* drop = new Item(dropS);
                drop->setPos(p->breakTarget);
                area->newItemToPlayers(drop);
                area->entities.addEntity(drop);
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
