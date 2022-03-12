#ifndef CHUNKCHANGETRACKER_H
#define CHUNKCHANGETRACKER_H

#include <list>
#include <thread>

#include "../../Datastructures/Coordinate/Coordinate.h"
#include "../../Datastructures/Coordinate/ChunkCoord.h"

#include "Block.h"
#include "ChunkChangeToken.h"

class Chunk; //Can't include here, is component of Chunk
class PlayerEntity;

class ChunkChangeTracker{
private:
    ChunkCoord cCoord;
    int numBlockChanges = 0;
    std::list<ChunkChangeToken*> changes;

    thread::id owner;

    bool casePlayerToken( std::list<ChunkChangeToken*>::iterator it, bool &passedData,
        int &newCompChunk, int index);

protected:
    // Testing only:
    ChunkChangeTracker(ChunkCoord coord, ChunkChangeToken* chunk);

public:
    ChunkChangeTracker(){};
    ChunkChangeTracker(Chunk*);

    void create(Chunk*);

    void sendChanges();

    //Position can be real or relative to chunk
    void addChange(Coordinate<int> pos, Block b);
    void addPlayer(PlayerEntity* p);

    //Important to remove player before deleting PlayerEntity
    void removePlayer(PlayerEntity* p);
    bool hasPlayers();
};

#endif
