#ifndef ITEM_H
#define ITEM_H

#include "Entity.h"

#include "../Inventory/Slot.h"

using namespace std;

class Block;

class Item : public Entity{
private:
    short blockToItem(short blockID);
public:
    //Note itemID is different from eid, itemID identifies what kind of item it is
    //while eid is simply a unique identifier
    int itemID;
    int count;
    int itemDamage;

    Item() : Entity(-1){};//Prevents a call to EIDAllocator for blank Entity
    Item(int it, int c = 1);
    Item(Slot &s);
    Item(Block b, Coordinate<int> pos);
    Item(Block b, Coordinate<double> pos);
    Item(Item*); //Identical, down to the eid. Not for copying items in-game

    EntityType getType(){return ITEM;};
    Item& operator=(Item &rhs);

    void sendToPlayer(PlayerEntity* p);

    Slot getSlot();

};

#endif
