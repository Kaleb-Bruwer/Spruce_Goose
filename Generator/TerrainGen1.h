#ifndef TERRAINGEN1_H
#define TERRAINGEN1_H

#include "GeneratorThread.h"

#include "./noise/PerlinNoise.h"
#include "../World/Chunk/Block.h"

using namespace std;

class TerrainGen1 : public GeneratorThread{
private:
    int seed;
public:
    TerrainGen1(BlockingQueueCounting<JobTicket*>* q, int seed);

    Chunk* generateChunk(ChunkCoord pos);
};

#endif
