#ifndef CHUNKFROMGENERATOR_H
#define CHUNKFROMGENERATOR_H

#include "../JobTicket.h"

using namespace std;

class Chunk;

class ChunkFromGenerator : public JobTicket{
public:
    Chunk* chunk = 0;

    string getType(){return "ChunkFromGenerator";};
};

#endif
