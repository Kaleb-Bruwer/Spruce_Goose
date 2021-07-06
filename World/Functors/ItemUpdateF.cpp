#include "ItemUpdateF.h"

#include <utility>

#include "../../JobTickets/WorldToProtocol/DestroyEntityJob.h"
#include "../../JobTickets/WorldToProtocol/SendItem.h"

#include "../EntityStore.h"

using namespace std;

ItemUpdateF::ItemUpdateF(EntityStore* s){
    store = s;
}

ItemUpdateF::ItemUpdateF(ItemPickupF &old, EntityStore *s){
    store = s;
    addItemPickupF(old);
}

void ItemUpdateF::addItemPickupF(ItemPickupF & old){
    for(Item* item : old.altered){
        altered.push_back(item);
    }
    for(int r : old.removed){
        removed.push_back(make_pair(r, old.player->eid));
    }
}

bool ItemUpdateF::mustRun(){
    return !(removed.empty() && altered.empty());
}

void ItemUpdateF::operator()(PlayerEntity* player){
    for(pair<int,int> r : removed){
        //Send delete jobs for these
        int eid = r.first;
        if(!player->hasEntity(eid))
            continue;
        player->removeEntity(eid);

        DestroyEntityJob* job = new DestroyEntityJob();
        job->eid = eid;
        job->pickupBy = r.second;
        player->pushJobToProtocol(job);
    }
    for(Item* i : altered){
        SendItem* job = new SendItem(i);
        job->newItem = false;
        job->item.position = Coordinate<double>(0,0,0);

        player->pushJobToProtocol(job);
    }
}
