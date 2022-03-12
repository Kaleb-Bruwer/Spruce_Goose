#ifndef ADDCHUNKSTOPLAYERF_H
#define ADDCHUNKSTOPLAYERF_H

#include "../../General/Functor.h"

using namespace std;

//For sending existing chunks to newly-loaded player

class PlayerEntity;
class EntityStore;
class Chunk;

class AddChunksToPlayerF : public Functor<Chunk*>{
protected:
    PlayerEntity* player;
    EntityStore* entities;
public:
    AddChunksToPlayerF(PlayerEntity* p, EntityStore* e);

    void operator()(Chunk* c);
};

#endif
