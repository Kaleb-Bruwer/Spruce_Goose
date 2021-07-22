#ifndef SUPERFLATGENERATOR_H
#define SUPERFLATGENERATOR_H

#include "GeneratorThread.h"

#include "../World/Chunk/Block.h"

using namespace std;

class SuperflatGenerator : public GeneratorThread{
private:
    vector<Block> layerBlocks; //Block for each layer
    vector<int> layerHeights; //How many y coordinates a layer covers
public:
    SuperflatGenerator(BlockingQueueCounting<JobTicket*>*);

    Chunk* generateChunk(ChunkCoord pos);
};

#endif
