#include "ItemPickupF.h"

#include <iostream>

#include "../../World/EntityStore.h"
#include "../../Entities/Entity.h"
#include "../../Entities/Item.h"
#include "../../Entities/PlayerEntity.h"

using namespace std;

ItemPickupF::ItemPickupF(PlayerEntity* p){
    player = p;
    pos = player->position;
}

void ItemPickupF::operator()(Entity* e){
    //First check entity type
    if(e->getType() != ITEM)
        return;

    Item* item = (Item*)e;

    if(e->position.getDist(pos) > pickupRadius)
        return;

    //If here, item is close enough to pick up.
    int pickedUp = player->inventory.inventory
        .tryPickup(item,player->inventory.alteredSlots);
    if(pickedUp == 0)
        return;

    if(item->count <= 0){
        item->mustDelete = true; //Will be deleted once functor exits
        removed.push_back(item->eid);
    }
    else{
        altered.push_back(item);
    }
}
