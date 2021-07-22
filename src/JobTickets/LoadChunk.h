#ifndef LOADCHUNK_H
#define LOADCHUNK_H

#include "JobTicket.h"
#include "../World/ThreadArea.h"

using namespace std;

class ThreadArea;

class LoadChunk : public JobTicket{
public:
    ChunkCoord chunkPos;
    ThreadArea* origin = 0;

    LoadChunk(ChunkCoord pos, ThreadArea* origin) : JobTicket(LOADCHUNK){
        chunkPos = pos;
        this->origin = origin;
    };
};

#endif
