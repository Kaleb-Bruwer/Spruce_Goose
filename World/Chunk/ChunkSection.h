#ifndef CHUNKSECTION_H
#define CHUNKSECTION_H

#include <vector>

#include "Block.h"
#include "../../Datastructures/Coordinate/Coordinate.h"
#include "../../Protocol/PacketWriter.h"

using namespace std;

class Chunk;

class ChunkSection{
protected:
    //Following constants are unlikely to change
    static const short sectionW = 16; //width (x,z)
    static const short sectionH = 16; //height (y)
    static const short sectionSize = 4096; //sectionW^2 * sectionH

    unsigned short nonAirCount = 0;
    Block blocks[sectionSize];

    friend class Chunk;

public:
    ChunkSection();
    ChunkSection(ChunkSection*);
    ~ChunkSection();

    Block getBlock(Coordinate<int> coord); //Assumed to be in right chunkSection
    void setBlock(Coordinate<int> coord, Block block);
    void setBlockRange(Coordinate<int> coord1, Coordinate<int> coord2, Block block);

    //sectionPos in [0,16), specifies which section this is as counted
    // from bottom
    bool fillHeightMap(unsigned short* heightMap, int sectionPos);
};

#endif
