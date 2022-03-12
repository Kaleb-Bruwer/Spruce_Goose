#ifndef CHUNKCHANGETOKEN_H
#define CHUNKCHANGETOKEN_H

#include <vector>

#include "Block.h"
#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

//Base class for any object that goes in the ChunkChangeTracker list
enum ChangeTokenType{
    BLOCKCHANGES,
    COMPCHUNK,
    PLAYERTOKEN
};

class ChunkChangeToken{
public:
    virtual ChangeTokenType getType() = 0;
};

#endif
