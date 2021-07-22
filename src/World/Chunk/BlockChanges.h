#ifndef BLOCKCHANGES_H
#define BLOCKCHANGES_H

#include "ChunkChangeToken.h"

#include "../../JobTickets/JobTicket.h"
#include "../../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

class BlockChanges : public ChunkChangeToken{
public:
    ChunkCoord cCoord;
    vector<Coordinate<int>> pos;
    vector<Block> blocks;

    //This gets dropped every time a new change is added
    //getJob will create it again if it's 0
    JobTicket* job = 0;

    BlockChanges(ChunkCoord c){cCoord = c;};

    void addChange(Coordinate<int> p, Block b);
    void merge(BlockChanges* rhs);

    ChangeTokenType getType(){return BLOCKCHANGES;};

    JobTicket* getJob();
};

#endif
