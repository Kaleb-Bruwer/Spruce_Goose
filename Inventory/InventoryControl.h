#pragma once

#include "AlteredSlots.h"
#include "Inventory2.h"

using namespace std;

class PlayerConnection;

class InventoryControl{
    PlayerConnection* conn = 0;
    BlockData* activeBlock = 0;
    int windowID = -1;


public:
    AlteredSlots alteredSlots;
    Inventory2 inventory;

    void clickWindow(ClickWindowJob* job, bool creative);

    //only sends if neccesary
    void sendWindowUpdate(PlayerConnection* c);

    void openBlock(BlockData* b);
    void closeBlock(bool byPlayer = true);


    void create(PlayerConnection* c){
        conn = c;
    }

    void setSlot(int i, Slot& s){
        if(i >= inventory.numSlots)
            return;

        inventory.slots[i] = s;
    };

    void setSlot(int i, Slot s){
        if(i >= inventory.numSlots)
            return;

        inventory.slots[i] = s;
    };


    Slot* getHeldItem(){
        return &inventory.slots[inventory.cursor];
    };

    short getHeldItemID(){
        return inventory.slots[inventory.cursor].itemID;
    };

    void decHeld(){//decrements held item by 1
        int c = inventory.cursor;
        inventory.slots[c].itemCount--;
        alteredSlots.add(c, inventory.slots[c]);
        if(inventory.slots[c].isEmpty()){
            inventory.slots[c].makeEmpty();
        }
    };

};
