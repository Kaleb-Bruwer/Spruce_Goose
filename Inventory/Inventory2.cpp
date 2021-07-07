#include "Inventory2.h"

#include <climits>
#include <cmath>

#include "Crafting/Crafting.h"
#include "Crafting/ShapedRecipe.h"

#include "../Entities/Item.h"
#include "../General/StackSizeTable.h"

BlockData* Inventory2::clone(){
    cout << "Inventory2::clone() not implemented";
    throw 0;
};

void Inventory2::craft(bool max, AlteredSlots &altered){
    /*
    This function assumes that there is already a valid crafting recipe in
    the crafting window and that it has its corresponding result in the product
    frame.

    Doesn't move result into main inventory, leaves it in slot[0] with an itemCount
    that can be greater than one stack
    The caller must move the result and call Crafting again to reset the crafting
    recipe
    */

    if(slots[0].isEmpty())
        return;

    int numToCraft = 1;

    if(max){
        int lowestLeft = slots[0].maxStackSize();
        for(int i=1; i < 5; i++){
            int temp = slots[i].itemCount;
            if(temp > 0 && temp < lowestLeft){
                lowestLeft = temp;
            }
        }
        if(lowestLeft == INT_MAX){
            slots[0].makeEmpty();
        }
        numToCraft = lowestLeft;
        //TODO: check how much space is left and lower numToCraft if neccesary

    }

    slots[0].itemCount *= numToCraft;
    altered.add(0, slots[0]);
    for(int i=1; i<5; i++){
        if(slots[i].itemCount == 0)
            continue;

        slots[i].itemCount -= numToCraft;
        if(slots[i].itemCount == 0)
            slots[i].makeEmpty();
        altered.add(i, slots[i]);
    }

    return;
}

void Inventory2::checkCrafting(AlteredSlots &altered){
    Crafting* crafting = Crafting::getInstance();

    ShapedRecipe r(2,2);
    r.setSlot(slots[1], 0, 0);
    r.setSlot(slots[2], 0, 1);
    r.setSlot(slots[3], 1, 0);
    r.setSlot(slots[4], 1, 1);

    slots[0] = crafting->getProduct(&r);
    altered.add(0, slots[0]);
}


void Inventory2::clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative){
    int i = job->slotNum;

    auto noShiftNoHover = [&](){
        if(i==0){
            //Craft 1 unit
            craft(false, altered);
            hover = slots[0];
            slots[0].makeEmpty();
            checkCrafting(altered);
            altered.add(0, slots[0]);
        }
        else{
            switch(job->button){
            case 0:
                //Pick up stack
                cout << "Pick up stack case: " << slots[i] << ", " << hover << endl;
                hover = slots[i];
                slots[i].makeEmpty();
                altered.add(i, slots[i]);
                break;

            case 1:
            {
                //Pick half up
                int take = ceil(slots[i].itemCount / 2.0);
                hover.itemID = slots[i].itemID;
                hover.itemDamage = slots[i].itemDamage;
                hover.itemCount = take;
                slots[i].itemCount -= take;
                altered.add(i, slots[i]);
                break;
            }
            case 3:
                //create new stack, creative mode only
                if(creative && !slots[i].isEmpty()){
                    hover = Slot(slots[i]);
                    hover.itemCount = hover.maxStackSize();
                    altered.add(i, slots[i]);
                }
            }
        }
    };

    auto noShiftHoverLeft = [&](){
        //Put down all in hover
        if(!slots[i].isEmpty()){
            //Does nothing if items aren't of the same type
            if(slots[i].itemID == hover.itemID
                    && slots[i].itemDamage == hover.itemDamage){

                int canMove = slots[i].maxStackSize() - slots[i].itemCount;
                canMove = min(canMove, (int) hover.itemCount);
                slots[i].itemCount += canMove;
                hover.itemCount -= canMove;
                if(hover.itemCount == 0){
                    hover.makeEmpty();
                }
                altered.add(i, slots[i]);
            }
            else{
                Slot temp = slots[i];
                slots[i] = hover;
                hover = temp;
                altered.add(i, slots[i]);
            }
        }
        else{
            //Empty slot, just move hover into it
            slots[i] = hover;
            hover.makeEmpty();
            altered.add(i, slots[i]);
        }
    };

    auto noShiftHoverRight = [&](){
        //Put down 1 item
        if(slots[i].isEmpty()){
            slots[i] = Slot();
            slots[i].itemID = hover.itemID;
            slots[i].itemDamage = hover.itemDamage;
        }
        else if(slots[i].itemID != hover.itemID
                || slots[i].itemDamage != hover.itemDamage){
            return;
        }

        if(slots[i].itemCount < slots[i].maxStackSize()){
            slots[i].itemCount++;
            hover.itemCount--;

            if(hover.itemCount == 0){
                hover.makeEmpty();
            }
        }
        altered.add(i, slots[i]);
    };

    auto shiftClick = [&](){
        if(job->button == 3)
        return;
        //Either movToMain or movToHotbar
        if(i == 0){
            //Craft max amount
            craft(true, altered);

            //The result from crafting is inserted differently from the inputs
            int slotOrder[36];
            int index = 0;
            for(int i=44; i>=9; i--){
                slotOrder[index++] = i;
            }
            mov(slots[0], 44, 9, altered);
            // mov((slotOrder[0]), 36, slots[0], 0);
            checkCrafting(altered);
        }
        else if(i >= 36){
            //movToMain
            mov(slots[i], 9, 35, altered);
        }
        else if(i >= 9 && i < 36){
            //movToHotbar(i);
            mov(slots[i], 36, 44, altered);
        }
        else{
            //movFromCraft(slots[i]);
            movHalf(slots[i], 44, 36, altered);
            movHalf(slots[i], 9, 35, altered);
            movEmpty(slots[i], 44, 36, altered);
            movEmpty(slots[i], 9, 35, altered);

            checkCrafting(altered);
        }
    };

    cout << (int) job->mode << ", " << (int)job->button << ": " << (int)job->slotNum << endl;

    switch(job->mode){
    case 0: //"normal" clicks
        if(job->button == 0){
            //Left click

            if(hover.isEmpty()){
                noShiftNoHover();
            }
            else{
                if(i == 0)
                    return;
                noShiftHoverLeft();
            }
        }
        else if(job->button == 1){
            //right click

            if(hover.isEmpty()){
                noShiftNoHover();
            }
            else{
                if(i ==0)
                    return;
                noShiftHoverRight();
            }
        }
        else{
            //undefined
            //Don't throw an error, this can be client-triggered
            cout << "Invalid button in Inventory2::clickWindow\n";
            return;
        }
        break;

    case 1: //shift click
        shiftClick();
        break;
    }

    if(i >= 1 && i <= 4){
        checkCrafting(altered);
    }
    return;
}


//Inventory iteration order: [36,45), [0,36)
int Inventory2::tryPickup(Item* item, AlteredSlots &altered){
    if(item->count == 0) return 0;
    int initialCount = item->count;

    Slot origin = item->getSlot();

    StackSizeTable* table = StackSizeTable::getInstance();
    int stackSize = table->getStackSize(item->itemID);

    movHalf(origin, 36, 44, altered);
    movHalf(origin, 9, 35, altered);
    movEmpty(origin, 36, 44, altered);
    movEmpty(origin, 9, 35, altered);
    item->count = origin.itemCount;

    return initialCount - origin.itemCount;
}

void Inventory2::close(AlteredSlots& a){
    // Flush crafting frame and hover
    for(int i=1; i<5; i++){
        if(slots[i].itemCount <= 0)
            continue;

        cout << "Clearing " << slots[i] << " from Inventory\n";
        movHalf(slots[i], 36, 44, a);
        movHalf(slots[i], 9, 35, a);
        movEmpty(slots[i], 36, 44, a);
        movEmpty(slots[i], 9, 35, a);

        if(slots[i].isEmpty()){
            slots[i].makeEmpty();
        }

        cout << "Leaving: " << slots[i] << endl;
    }

    if(hover.itemCount <= 0)
        return;

    cout << "Clearing " << hover << " from Inventory\n";
    movHalf(hover, 36, 44, a);
    movHalf(hover, 9, 35, a);
    movEmpty(hover, 36, 44, a);
    movEmpty(hover, 9, 35, a);

    if(hover.isEmpty()){
        hover.makeEmpty();
    }

    cout << "Leaving: " << hover << endl;


}
