#pragma once

#include "AdvancedWriter.h"

#include <vector>

#include "../World/Chunk/Block.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

class BlockChangeWriter : public AdvancedWriter{
protected:

public:
    BlockChangeWriter(unsigned int size = 256);

    void writeBlockChanges(ChunkCoord cCoord, vector<Coordinate<int>>& pos, vector<Block>& blocks);
};
