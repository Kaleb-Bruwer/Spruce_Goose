#ifndef BLOCKUPDATETOPLAYER_H
#define BLOCKUPDATETOPLAYER_H

#include "../JobTicket.h"

#include "../../World/Chunk/Block.h"
#include "../../Datastructures/Coordinate/Coordinate.h"
#include "../../Entities/Item.h"

using namespace std;

class BlockUpdateToPlayer : public JobTicket{
public:
    //PlayerConnection will have to separate block changes into chunks
    BlockUpdateToPlayer(){};
    BlockUpdateToPlayer(BlockUpdateToPlayer *b){
        positions = b->positions;
        blocks = b->blocks;
    };

    vector<Coordinate<int>> positions;
    vector<Block> blocks;

    string getType(){return "BlockUpdateToPlayer";};

};

#endif
