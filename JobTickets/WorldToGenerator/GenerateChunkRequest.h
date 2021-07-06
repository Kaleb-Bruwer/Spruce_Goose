#ifndef GENERATECHUNKREQUEST_H
#define GENERATECHUNKREQUEST_H

#include "../JobTicket.h"
#include "../../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

class SynchedArea;

class GenerateChunkRequest : public JobTicket{
public:
    ChunkCoord chunkPos;
    SynchedArea* origin = 0;

    string getType(){return "GenerateChunkRequest";};
};

#endif
