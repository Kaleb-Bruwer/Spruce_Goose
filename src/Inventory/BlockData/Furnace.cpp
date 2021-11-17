#include "Furnace.h"

#include <cmath>

#include "../Inventory2.h"
#include "../Crafting/Crafting.h"

using namespace std;

BlockData* Furnace::clone(){

}

vector<Slot> Furnace::clickWindow(ClickWindowJob* job, Inventory2* inv,
        AlteredSlots &altered, bool creative){
    // 0: input
    // 1: fuel
    // 2: output
    vector<Slot> dropped;

    int clicked = job->slotNum;
    int btn = job->button;
    int mode = job->mode;

    Slot& hover = inv->hover;

    // Handle modes that can't be passed down to inventory here
    switch(mode){
    case 1:{
        // Shift click
        if(clicked < 0 | clicked > 38) //Validity check
            break;

        if(clicked > 2){
            //inv -> furnace
            Slot& origin = inv->slots[clicked + 6]; //6 is the offset for furnace
            if(origin.isEmpty())
                break; //Don't waste time in this case

            Crafting* crafting = Crafting::getInstance();
            Slot result = crafting->getSmeltingProduct(origin);

            int targetSlot = -1;

            if(!result.isEmpty()){
                // move into input
                targetSlot = 0;
            }
            else if(result.isFuel()){
                // move into fuel
                targetSlot = 1;
            }
            else{
                // Between inventory and hotbar
                altered.setOffset(-6);
                job->slotNum += 6;
                inv->clickWindow(job, 0, altered, creative);
                altered.setOffset(0);
                break;
            }

            Slot& target = slots[targetSlot];
            if(target.isEmpty()){
                target = origin;
                origin.makeEmpty();
                altered.add(targetSlot, target);
            }
            else if(target.typeMatch(origin)){
                int take = target.maxStackSize() - target.itemCount;
                take = min(take, (int) origin.itemCount);

                origin.itemCount -= take;
                target.itemCount += take;

                if(origin.isEmpty())
                    origin.makeEmpty();

                altered.add(targetSlot, target);
            }
        }
        else{
            // furnace -> inv
            altered.setOffset(-6);
            inv->mov(slots[clicked], 9, 44, altered);
            altered.setOffset(0);
            altered.add(clicked, slots[clicked]);
        }

        break;
    }

    default:
        if(clicked < 3){
            switch(mode){
            case 0:{ //left & right click
                if(clicked < 0) //validation check
                    break;

                Slot& origin = slots[clicked];

                if(clicked == 0){
                    // Output is a special case: nothing can be placed here
                    if(btn != 0 && btn != 1) //validation check
                        break;

                    if(hover.isEmpty()){
                        hover = origin;
                        origin.makeEmpty();
                        altered.add(0, origin);
                    }
                    else if(hover.typeMatch(origin) &&
                            (hover.maxStackSize() - hover.itemCount >= origin.itemCount)){
                        hover.itemCount += origin.itemCount;
                        origin.makeEmpty();
                        altered.add(0, origin);
                    }

                } //After this you can assume it's a placable slot
                else if(btn == 0){ //left click
                    if(origin.isEmpty() || hover.isEmpty() || !origin.typeMatch(hover)){
                        // Swap slots
                        Slot temp = origin;
                        origin = hover;
                        hover = temp;
                        altered.add(clicked, origin);
                    }
                    else{ //Place extra, same type
                        int stackSize = origin.maxStackSize();
                        int take = min(stackSize - origin.itemCount, (int)hover.itemCount);
                        hover.itemCount -= take;
                        origin.itemCount += take;

                        if(hover.isEmpty())
                            hover.makeEmpty();
                        altered.add(clicked, origin);
                    }
                }
                else if(btn == 1){ //right click
                    if(hover.isEmpty() && !origin.isEmpty()){
                        // Pick up half
                        int take = ceil(origin.itemCount / 2.0);
                        hover.itemID = origin.itemID;
                        hover.itemDamage = origin.itemDamage;
                        hover.itemCount = take;
                        origin.itemCount -= take;

                        if(origin.isEmpty())
                            origin.makeEmpty();

                        altered.add(clicked, origin);
                        break;

                    }
                    else if(!hover.isEmpty() && (origin.isEmpty() || origin.typeMatch(hover))){
                        int oldCount = origin.itemCount;
                        origin = hover;
                        origin.itemCount = oldCount + 1;
                        hover.itemCount--;
                        if(hover.isEmpty())
                            hover.makeEmpty();

                        altered.add(clicked, origin);

                    }
                }
                break;
            }
            case 2:
                dropped = clickMode2(clicked, btn, inv, altered, creative);
                break;
            case 3:
                if(creative && clicked >= 0 && hover.isEmpty()){
                    hover = slots[clicked];
                    hover.itemCount = hover.maxStackSize();
                }
                break;
            case 4:{ //Drop item(s)
                if(clicked < 0 || slots[clicked].isEmpty())
                    break;

                Slot& origin = slots[clicked];
                // Buttons 0 & 1 with clicked = -999 are in protocol, but as no-op
                if(btn == 0){
                    //Drop one
                    Slot drop = origin;
                    drop.itemCount = 1;
                    origin.itemCount--;
                    if(origin.isEmpty())
                        origin.makeEmpty();

                    dropped.push_back(drop);
                    altered.add(clicked, origin);
                }
                else if(job->button == 1){
                    //Drop stack
                    dropped.push_back(origin);
                    origin.makeEmpty();
                    altered.add(clicked, Slot());
                }
                break;
            }
            }
        }
        else{
            altered.setOffset(-6);
            job->slotNum += 6;
            dropped = inv->clickWindow(job, inv, altered, creative);
            altered.setOffset(0);
        }
    };
    return dropped;
}

vector<Slot> Furnace::clickMode2(int clicked, int btn, Inventory2* inv,
        AlteredSlots &altered, bool creative){
    // Can assume clicked < 3

    if(btn < 0 || btn > 8) //validity cheks
        return vector<Slot>();
    if(clicked < 0)
        return vector<Slot>();
    if(!inv->hover.isEmpty())
        return vector<Slot>();

    Slot& origin = slots[clicked];
    Slot& target = inv->slots[36 + btn];

    if(target.isEmpty() || origin.isEmpty()){
        // Similar to mode 2 in chest (move target out of the way)
        if(clicked == 2 && !target.isEmpty())
            return vector<Slot>(); //nothing to move out
        else{
            Slot temp = target;
            target = origin;
            origin = temp;
            altered.add(clicked, origin);
            altered.add(30 + btn, target);
        }
    }
    else{
        // Can only move if there's an open slot somewhere
        // in inventory (yes, seriously...)
        bool found = false;
        for(int i=9; i<45; i++){
            if(inv->slots[i].isEmpty()){
                found = true;
                break;
            }
        }

        if(!found) //no-op scenario
            return vector<Slot>();

        if(target.typeMatch(origin)){
            // Same type
            int take = target.maxStackSize() - target.itemCount;
            take = min(take, (int) origin.itemCount);

            origin.itemCount -= take;
            target.itemCount += take;
            altered.add(30 + btn, target);

            if(origin.isEmpty())
                origin.makeEmpty();
            else{
                // Place what's left of origin on some open slot
                // First check hotbar
                altered.setOffset(-6);
                inv->movHalf(origin, 36, 44, altered);
                if(!origin.isEmpty()){
                    inv->movHalf(origin, 9, 35, altered);
                    if(!origin.isEmpty()){
                        inv->movEmpty(origin, 36, 44, altered);
                        if(!origin.isEmpty())
                            inv->movEmpty(origin, 9, 35, altered);
                    }
                }
                altered.setOffset(0);
            }
            altered.add(clicked, origin);
        }
        else{
            // Different types
            // Move target out first
            altered.setOffset(-6);
            inv->movHalf(target, 36, 44, altered);
            if(!origin.isEmpty()){
                inv->movHalf(target, 9, 35, altered);
                if(!origin.isEmpty()){
                    inv->movEmpty(target, 36, 44, altered);
                    if(!origin.isEmpty())
                        inv->movEmpty(target, 9, 35, altered);
                }
            }
            altered.setOffset(0);

            // Now move origin to target
            target = origin;
            origin.makeEmpty();
            altered.add(clicked, origin);
            altered.add(30 + btn, target);
        }
    }

    return vector<Slot>(); //dropped
}
