#include "Chest.h"

#include <cstring>
#include <cmath>

#include "../Inventory2.h"

using namespace std;

BlockData* Chest::clone(){
    Chest* result = new Chest();
    memcpy(result->slots, slots, sizeof(slots));
    return result;
}

vector<Slot> Chest::clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative){
    // 0 to 26 is the chest
    // 27 to 53 is the main inventory
    // 54 to 62 is the hotbar

    vector<Slot> dropped;

    Slot& origin = slots[0]; //meaningless value, references just have to be initialized
    bool inChest = false;
    int i = job->slotNum;

    if(i <= 26){
        inChest = true;
        origin = slots[i];
    }
    else{
        origin = inv->slots[i-18];
    }

    if(job->shift){
        if(job->button == 3)
            return dropped;

        //if inChest, move to inventory
        //If !inChest, move to chest
        if(inChest){
            //invMov(inv, invOrder, 27, origin, 18);
            altered.setOffset(18);
            inv->mov(origin, 44, 9, altered);
            altered.setOffset(0);
            altered.add(i, origin);
        }
        else{
            //movToChest(origin);
            mov(origin, 0, 26, altered);
            altered.add(i, origin);
        }

    }
    else{
        Slot& hover = inv->hover;
        switch(job->button){
        case 0:
            if(!hover.isEmpty()){
                //Player is holding something
                int stackSize = hover.maxStackSize();
                tryInsertHalfSlot(origin, hover, stackSize);
                if(!hover.isEmpty())
                    tryInsertEmptySlot(origin, hover, stackSize);

                altered.add(i, origin);
            }
            else{
                //Player is holding nothing
                if(!origin.isEmpty()){
                    hover = origin;
                    origin.makeEmpty();
                    altered.add(i, origin);
                }
            }
            break;
        case 1:
            if(!hover.isEmpty()){
                if(origin.isEmpty()){
                    origin = hover;
                    origin.itemCount = 1;
                    hover.itemCount--;
                    if(hover.isEmpty())
                        hover.makeEmpty();
                    altered.add(i, origin);
                }
            }
            else{
                int take = ceil((double) origin.itemCount / 2);
                if(take == 0)
                    return dropped;
                hover = origin;
                hover.itemCount = take;
                origin.itemCount -= take;
                altered.add(i, origin);
            }
            break;
        case 3:
            if(hover.isEmpty() && !origin.isEmpty() && creative){
                hover = origin;
                hover.itemCount = hover.maxStackSize();
            }
        }
    }

    return dropped;
}
