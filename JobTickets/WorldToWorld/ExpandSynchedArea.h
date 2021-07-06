#ifndef EXPANDSYNCHEDAREA_H
#define EXPANDSYNCHEDAREA_H

#include "../JobTicket.h"

using namespace std;

#include "../../Datastructures/Coordinate/ChunkCoord.h"

class SynchedArea;
class ThreadArea;

//World will use this to expand the specified SynchedArea
//And will rerun claimChunk on the new and expanded SynchedArea

class ExpandSynchedArea : public JobTicket{
public:
    ChunkCoord coord; //Chunk, not region
    SynchedArea* synchedArea = 0;
    ThreadArea* claimer = 0;

    string getType(){return "ExpandSynchedArea";};
};

#endif
