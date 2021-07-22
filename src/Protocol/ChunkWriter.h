#pragma once

#include "AdvancedWriter.h"

using namespace std;

class Chunk;

class ChunkWriter : public AdvancedWriter{
protected:
    //returns size of compressed data
    int writeData(Chunk* c);
public:
    ChunkWriter(unsigned int size = 256);

    void writeChunk(Chunk*);

};
