#ifndef REGION_H
#define REGION_H

#include <string>

#include "Chunk/Chunk.h"
#include "Chunk/CompressedChunk.h"
#include "../Datastructures/Coordinate/RegionCoord.h"

using namespace std;

//Region keeps compressed copies of all chunks
//It replaces these copies when it gets new ones from ThreadArea
//This means that it can still write back all chunks that were loaded

//LocationTable is broken up into fileLocation objects
struct fileLocation{
    unsigned int offset;
    unsigned char size;
    unsigned char x;
    unsigned char z;
};

class Region{
private:
    RegionCoord rCoord;
    Chunk* chunks[32][32]; //(z,x)
    CompressedChunk* compressed[32][32]; //(z,x)

    void commonConstructor();
    string coordToFileName(RegionCoord r);
    string getFileNameDir(RegionCoord r); //Also adds directory to file name
    string getFileNameDir();

public:
    Region();
    Region(RegionCoord r);

    //Returns false if file doesn't exist
    bool read(RegionCoord r);
    void write();//Writes back entire region

    Chunk* getChunk(ChunkCoord c);
    void addChunk(Chunk* c);//Newly generated chunks get added here
    void addChunk(CompressedChunk* c);
};

#endif
