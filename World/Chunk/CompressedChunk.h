#ifndef COMPRESSEDCHUNK_H
#define COMPRESSEDCHUNK_H

#include "../../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

class Chunk;
class Region;

class CompressedChunk{
private:
    ChunkCoord coord;
    unsigned int len = 0;
    char* data = 0;
    char compressionScheme = 0; //1=gzip, 2=zlib. Only zlib is actually used

    friend class Region;
public:
    CompressedChunk(char* raw, ChunkCoord coord); //Ptr to start of raw chunk, including header
    CompressedChunk(Chunk* c);
    ~CompressedChunk();

    void writeHeader(char*);//Writes 5 bytes that come before compressed data
    int getSizeInBlocks();//Size as n with n ceil((len+5)/4096)
    ChunkCoord getChunkCoord(){return coord;};

    Chunk* getChunk(); //Decompresses data and create Chunk
};

#endif
