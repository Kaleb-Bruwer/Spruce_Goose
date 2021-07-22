#include "PlayerPickupF.h"

#include <iostream>

#include "ItemUpdateF.h"
#include "ItemPickupF.h"
#include "../EntityStore.h"

#include "../../Entities/PlayerEntity.h"

using namespace std;

PlayerPickupF::PlayerPickupF(ItemUpdateF* u, EntityStore* s){
    updater = u;
    store = s;
}


void PlayerPickupF::operator()(PlayerEntity* player){
    if(rand() % 10 == 0){    
        ItemPickupF ipF(player);
        store->execFuncInRadius(player->position, 1.425, ipF);
        updater->addItemPickupF(ipF);//This one keeps track of changes
    }
}
