#ifndef BLOCKUPDATESF_H
#define BLOCKUPDATESF_H

#include "../../General/Functor.h"
#include "../Chunk/Chunk.h"

using namespace std;

class EntityStore;
class PlayerEntity;
class BlockUpdateToPlayer;

class BlockUpdatesF : public Functor<Chunk*>{
public:
    BlockUpdatesF(){};

    void operator()(Chunk* c){c->sendChanges();};
};

#endif
