#ifndef PLAYERPICKUPF_H
#define PLAYERPICKUPF_H

#include "../../General/Functor.h"

using namespace std;

class ItemUpdateF;

/*
This functor is created once a tick and called on all players
the () operator creates an ItemPickupF which in turn gets called on all
relevant Entities. Then that ItemPickupF gets passed to updater to track any
changes it made.
*/

class PlayerEntity;
class EntityStore;

class PlayerPickupF : public Functor<PlayerEntity*>{
private:
    ItemUpdateF* updater;
    EntityStore* store;

public:
    PlayerPickupF(ItemUpdateF* u, EntityStore* s);

    void operator()(PlayerEntity* p);
};

#endif
