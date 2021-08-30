#include "CraftingTable.h"

#include <iostream>
#include <cstring>
#include <cmath>

#include "../Inventory2.h"
#include "../Crafting/Crafting.h"
#include "../Crafting/ShapedRecipe.h"

using namespace std;

BlockData* CraftingTable::clone(){
    CraftingTable* result = new CraftingTable();
    memcpy(result->slots, slots, sizeof(slots));
    return result;
}

void CraftingTable::craft(bool max, AlteredSlots &altered){
    //This function doesn't "calculate" a crafting recipe, it simply executes it

    if(slots[0].isEmpty())
        return;

    int numToCraft = 1;

    if(max){
        int lowestLeft = slots[0].maxStackSize();
        for(int i=1; i<10; i++){
            int temp = slots[i].itemCount;
            if(temp > 0 && temp < lowestLeft)
                lowestLeft = temp;
        }
        //TODO: check how much space is left and lower numToCraft if neccesary
        numToCraft = lowestLeft;
    }

    //Multipled by how many items you get per craft, i.e. planks give 4 at once
    slots[0].itemCount *= numToCraft;

    for(int i=1; i<10; i++){
        if(slots[i].itemCount != 0){
            slots[i].itemCount -= numToCraft;
            if(slots[i].isEmpty())
                slots[i].makeEmpty();
            altered.add(i, slots[i]);
        }
    }
    altered.add(0, slots[0]);
}

void CraftingTable::checkCrafting(AlteredSlots &altered){
    Crafting* crafting = Crafting::getInstance();

    ShapedRecipe r(3,3);
    r.setSlot(slots[1], 0, 0);
    r.setSlot(slots[2], 0, 1);
    r.setSlot(slots[3], 0, 2);

    r.setSlot(slots[4], 1, 0);
    r.setSlot(slots[5], 1, 1);
    r.setSlot(slots[6], 1, 2);

    r.setSlot(slots[7], 2, 0);
    r.setSlot(slots[8], 2, 1);
    r.setSlot(slots[9], 2, 2);

    slots[0] = crafting->getProduct(&r);
    altered.add(0, slots[0]);
}

// void CraftingTable::clickWindow(ClickWindowJob* job, Inventory* inv, bool creative){
vector<Slot> CraftingTable::clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative){
    vector<Slot> dropped;

    int clicked = job->slotNum;
    Slot& hover = inv->hover;

    if(clicked < 0){
        return dropped; //invalid request
    }

    if(clicked < 10){
        // Action involves crafting window
        Slot& origin = slots[clicked];

        switch(job->mode){
        case 0:
            //normal click
            if(i == 0){
                // crafting output
                // TODO: crafting
            }
            else{
                // crafting frame
                Slot temp = hover;
                hover = origin;
                origin = temp;
                altered.add(clicked, origin);
            }
            break;
        case 1:
            //shift click
            if(job->button != 0 && job->button != 1) //invalid request
                return dropped;

            if(i==0){
                // TODO: crafting
            }
            else{
                //crafting frame
                if(!origin.isEmpty()){
                    altered.setOffset(-1);
                    inv->mov(origin, 9, 44, altered);
                    altered.setOffset(0);
                    altered.add(clicked, origin);
                }
            }
            break;
        case 2:
            //number keys
            break;
        case 4:
            //middle click
            break;
        case 5:
            //mouse drags
            break;
        case 6:
            //double click
            break;
        }
    }
    else{
        //Clicked in inventory
        //Simply call Inventory2::clickInventory, since this case is exactly the
        //same as that
        //In future versions of MC this changes

        altered.setOffset(-1); //slot numbers are offset from normal inventory
        job->slotNum--;
        dropped = inv->clickWindow(job, inv, altered, creative);
        altered.setOffset(0);
    }

    return dropped;
}
