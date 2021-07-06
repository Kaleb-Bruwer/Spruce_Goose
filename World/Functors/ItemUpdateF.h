#ifndef ITEMUPDATEF_H
#define ITEMUPDATEF_H

#include <vector>

#include "../../General/Functor.h"
#include "ItemPickupF.h"

using namespace std;

class EntityStore;

//IMPORTANT: this class does not send any new items to players,
//it only updates items they already have
class ItemUpdateF : public Functor<PlayerEntity*>{
private:
    EntityStore* store;
    vector<Item*> altered;//These are split form each other in constructor
    vector<pair<int, int>> removed; //<id of removed item, player who picked it up>

public:
    ItemUpdateF(EntityStore* store);
    ItemUpdateF(ItemPickupF &old, EntityStore *store);

    //This is so that you can send item changes caused by >1 players' pickups at once
    void addItemPickupF(ItemPickupF &old);

    void operator()(PlayerEntity* e);

    //If mustRun returns false, don't run this
    bool mustRun();
};

#endif
