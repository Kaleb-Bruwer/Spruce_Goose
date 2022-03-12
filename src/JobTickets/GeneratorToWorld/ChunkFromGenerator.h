#ifndef CHUNKFROMGENERATOR_H
#define CHUNKFROMGENERATOR_H

#include "../JobTicket.h"

using namespace std;

class Chunk;

class ChunkFromGenerator : public JobTicket{
public:
    ChunkFromGenerator() : JobTicket(CHUNKFROMGENERATOR){};

    Chunk* chunk = 0;
};

#endif
