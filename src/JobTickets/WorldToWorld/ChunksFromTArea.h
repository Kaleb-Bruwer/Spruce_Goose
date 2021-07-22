#ifndef CHUNKSFROMTAREA_H
#define CHUNKSFROMTAREA_H

#include "../JobTicket.h"
#include "../../World/Chunk/Chunk.h"

using namespace std;

class ChunksFromTArea : public JobTicket{
public:
    ChunksFromTArea() : JobTicket(CHUNKSFROMTAREA){};

    bool forAutosave = false;
    Chunk** chunks = 0;
    int numChunks = 0;

    ~ChunksFromTArea(){
        if(chunks)
            delete [] chunks;
    };
};

#endif
