#include "Item.h"

#include "../JobTickets/WorldToProtocol/SendItem.h"
#include "../World/Chunk/Block.h"

#include "PlayerEntity.h"

using namespace std;

Item::Item(int it, int c) : Entity(){
    itemID = it;
    count = c;
};

Item::Item(Slot &s) : Entity(){
    itemID = s.itemID;
    count = s.itemCount;
    itemDamage = s.itemDamage;
};

Item::Item(Block b, Coordinate<double> pos) : Entity(){
    itemID = blockToItem(b.id);
    itemDamage = b.metadata;
    count = 1;

    position.x = pos.x;
    position.y = pos.y;
    position.z = pos.z;

    oldPosition = position;
}

Item::Item(Block b, Coordinate<int> pos) : Entity(){
    itemID = blockToItem(b.id);
    itemDamage = b.metadata;
    count = 1;

    position.x = pos.x + 0.5;
    position.y = pos.y + 0.5;
    position.z = pos.z + 0.5;

    oldPosition = position;
}

Item::Item(Item* rhs) : Entity(rhs){
    itemID = rhs->itemID;
    count = rhs->count;
    itemDamage = rhs->itemDamage;
}

short Item::blockToItem(short bID){
    return bID;
    //TODO: special cases?
}

Item& Item::operator=(Item &rhs){
    *((Entity*)this) = *((Entity*)&rhs);

    itemID = rhs.itemID;
    count = rhs.count;
    itemDamage = rhs.itemDamage;

    return *this;
}

Slot Item::getSlot(){
    Slot result;
    result.itemID = itemID;
    result.itemCount = count;
    result.itemDamage = itemDamage;

    return result;
}

void Item::sendToPlayer(PlayerEntity* p){
    Entity::sendToPlayer(p);
    SendItem* job = new SendItem(this);
    p->pushJobToProtocol(job);
}
