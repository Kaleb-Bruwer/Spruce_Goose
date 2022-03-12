#pragma once

#include "../../Datastructures/Coordinate/ChunkCoord.h"
#include "../../Protocol/PacketReader.h"

struct RawChunkMeta{
    ChunkCoord coord;
    short int bitmask;
    short int addBitmask;
};

class Chunk;

class MapChunkBulkReader : public PacketReader{
private:
    //If this object shadows another PacketReader, it can't delete its data
    bool ownData;
public:
    MapChunkBulkReader(char* start, int size) : PacketReader(start, size){
        ownData = true;
    };
    MapChunkBulkReader(PacketReader &p);
    ~MapChunkBulkReader();

    // end: end index of this packet (excluded)
    void readAll(int end, vector<Chunk*> &dest);
};
