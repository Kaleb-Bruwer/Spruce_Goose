#ifndef COORDAREAPAIR_H
#define COORDAREAPAIR_H

#include "../Coordinate/ChunkCoord.h"

class ThreadArea;

class CoordAreaPair{
public:
    ChunkCoord chunkCoord;
    ThreadArea* threadArea;

    CoordAreaPair(){};
    CoordAreaPair(ChunkCoord, ThreadArea*);
    CoordAreaPair& operator=(CoordAreaPair& rhs);
};

#endif
