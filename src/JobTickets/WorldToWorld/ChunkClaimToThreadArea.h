#ifndef CHUNKCLAIMTOTHREADAREA_H
#define CHUNKCLAIMTOTHREADAREA_H

#include "../JobTicket.h"

using namespace std;

#include "../../Datastructures/Coordinate/ChunkCoord.h"

class SynchedArea;
class ThreadArea;

//World will use this to expand the specified SynchedArea
//And will rerun claimChunk on the new and expanded SynchedArea

class ChunkClaimToThreadArea : public JobTicket{
public:
    ChunkClaimToThreadArea() : JobTicket(CHUNKCLAINTOTHREADAREA){};

    ChunkCoord coord; //Chunk, not region
    ThreadArea* owner = 0;
};

#endif
