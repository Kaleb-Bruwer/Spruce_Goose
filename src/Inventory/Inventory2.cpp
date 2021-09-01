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


vector<Slot> Inventory2::clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative){
    vector<Slot> dropped;

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
                // cout << "Pick up stack case: " << slots[i] << ", " << hover << endl;
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

                if(slots[i].isEmpty())
                    slots[i].makeEmpty();

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
            Slot temp = slots[i];
            slots[i] = hover;
            hover = temp;
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
            for(int j=44; j>=9; j--){
                slotOrder[index++] = j;
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
        altered.add(i, slots[i]);
    };

    switch(job->mode){
    case 0: //"normal" clicks
        if(i < 0){ //spec says -999, but making it <0 should be harmless
            if(hover.isEmpty())
                break;
            if(job->button == 0){
                //Drop all in hover
                dropped.push_back(hover);
                hover.makeEmpty();
            }
            else if(job->button == 1){
                //Drop one from hover
                Slot drop = hover;
                drop.itemCount = 1;
                hover.itemCount--;
                if(hover.isEmpty())
                    hover.makeEmpty();

                dropped.push_back(drop);
            }
            break;
        }
        if(i >= 45) //invalid
            break;

        if(job->button == 0){
            //Left click

            if(hover.isEmpty()){
                noShiftNoHover();
            }
            else{
                if(i == 0)
                    return dropped;
                noShiftHoverLeft();
            }
        }
        else if(job->button == 1){
            //right click

            if(hover.isEmpty()){
                noShiftNoHover();
            }
            else{
                if(i == 0)
                    return dropped;
                noShiftHoverRight();
            }
        }
        else{
            //undefined
            //Don't throw an error, this can be client-triggered
            cout << "Invalid button in Inventory2::clickWindow\n";
            return dropped;
        }
        break;

    case 1: //shift click
        if(i < 0 || i >44) //invalid
            break;
        shiftClick();
        break;
    case 2:{ //number key
        if(!hover.isEmpty() || i == 0)
            break;

        if(job->button < 0 || job->button > 8)
            break; //invalid request

        if(i < 0 || i > 44) //invalid
            break;

        int targetSlot = job->button + 36;
        Slot temp = slots[targetSlot];
        slots[targetSlot] = slots[i];
        slots[i] = temp;
        altered.add(targetSlot, slots[targetSlot]);
        altered.add(i, slots[i]);
        break;
    }
    case 3: //creative mode only, duplicate a stack
        if(i < 0 || i > 44) //invalid
            break;
        if(creative && hover.isEmpty() && job->button == 2){
            hover = slots[i];
            hover.itemCount = hover.maxStackSize();
        }
        break;
    case 4:
        // Buttons 0 & 1 with i = -999 are in protocol, but as no-op
        if(i < 0 || i > 44) //invalid
            break;
        if(job->button == 0){
            //Drop one
            Slot drop = slots[i];
            drop.itemCount = 1;
            slots[i].itemCount--;
            if(slots[i].isEmpty())
                slots[i].makeEmpty();

            dropped.push_back(drop);
            altered.add(i, slots[i]);
        }
        else if(job->button == 1){
            //Drop stack
            dropped.push_back(slots[i]);
            slots[i].makeEmpty();
            altered.add(i, slots[i]);
        }
        break;
    case 5: //mouse drags
        switch(job->button){
            case 0: //start left drag
                dragData.dragMode = LEFT;
                dragData.dragTotal = hover.itemCount;
                break;

            case 1: //add slot to left drag
            // First check some conditions
            if(i <0 || i > 44) //invalid
                break;
            if(dragData.dragMode == LEFT){
                if(!slots[i].isEmpty() && !slots[i].typeMatch(hover))
                    break;

                dragData.dragSlots.push_back(i);
                dragData.baseCount.push_back(slots[i].itemCount);

                // Conditions where no action is required
                int numSlotsInDrag = dragData.dragSlots.size();
                if(numSlotsInDrag <= 1 || numSlotsInDrag > dragData.dragTotal)
                    break;

                // Split from scrach every time
                // numToAdd: num added to each slot in dragSlots
                int numToAdd = floor((float)dragData.dragTotal/numSlotsInDrag);
                int remainder = dragData.dragTotal - numToAdd * numSlotsInDrag;

                int maxStack = hover.maxStackSize();

                for(int i=0; i<dragData.dragSlots.size(); i++){
                    int s = dragData.dragSlots[i];
                    slots[s] = hover; // sets the type
                    slots[s].itemCount = dragData.baseCount[i] + numToAdd;
                    if(slots[s].itemCount > maxStack){
                        remainder += slots[s].itemCount - maxStack;
                        slots[s].itemCount = maxStack;
                    }
                    altered.add(s, slots[s]);
                }
                hover.itemCount = remainder;
            }
            break;

            case 2: //end left drag
                dragData.dragMode = NONE;

                if(dragData.dragSlots.size() == 1){
                    int s = dragData.dragSlots[0];
                    slots[s] = hover;
                    hover.makeEmpty();
                }
                if(hover.isEmpty())
                    hover.makeEmpty();

                dragData.dragSlots.clear();
                dragData.baseCount.clear();
                break;


            case 4: //start right drag
                dragData.dragMode = RIGHT;
                dragData.dragTotal = hover.itemCount;
            break;

            case 5: //add slot to right drag
            if(i <0 || i > 44) //invalid
                break;
            if(dragData.dragMode == RIGHT){
                if(!slots[i].isEmpty() && !slots[i].typeMatch(hover))
                    break;

                // Conditions where no action is required
                if(hover.itemCount == 0)
                    break;

                // Add one item to new slot (if possible)
                int maxStack = hover.maxStackSize();
                if(slots[i].itemCount < maxStack){
                    slots[i].itemID = hover.itemID;
                    slots[i].itemDamage = hover.itemDamage;
                    slots[i].itemCount++;
                    hover.itemCount--;
                    if(hover.itemCount == 0)
                        hover.makeEmpty();
                }
                altered.add(i, slots[i]);
            }
            break;

            case 6: //end right drag

                dragData.dragMode = NONE;
            break;
        }
        break;
    case 6: { //double click
        if(i <0 || i > 44) //invalid
            break;

        int stackLimit = hover.maxStackSize();
        // Inside inventory, not armour and not crafting result
        if((i >= 1 && i < 45) && !(i >= 5 && i < 9)){
            if(hover.isEmpty())
                break;
            // Fill from incomplete stacks
            for(int j=9; j < 45; j++){
                if(slots[j].typeMatch(hover) && slots[j].itemCount < stackLimit){
                    tryInsertHalfSlot(hover, slots[j], stackLimit);
                    altered.add(j, slots[j]);
                    if(hover.itemCount == stackLimit)
                        break;
                }
            }
            // Fill from full stacks
            for(int j=9; j < 45; j++){
                if(slots[j].typeMatch(hover)){
                    tryInsertHalfSlot(hover, slots[j], stackLimit);
                    altered.add(j, slots[j]);
                    if(hover.itemCount == stackLimit)
                        break;
                }
            }
        }
        break;
    }

    }

    if(i >= 1 && i <= 4){
        checkCrafting(altered);
    }
    return dropped;
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

void Inventory2::moveIn(Slot& origin, AlteredSlots& altered){
    movHalf(origin, 36, 44, altered);
    movHalf(origin, 9, 35, altered);
    movEmpty(origin, 36, 44, altered);
    movEmpty(origin, 9, 35, altered);

    if(origin.isEmpty())
        origin.makeEmpty();
}


vector<Slot> Inventory2::close(InventoryControl* invCon, AlteredSlots& altered, Inventory2* inv){
    vector<Slot> dropped;

    // Flush crafting frame and hover
    for(int i=1; i<5; i++){
        if(slots[i].itemCount <= 0)
            continue;

        moveIn(slots[i], altered);

        if(!slots[i].isEmpty()){
            dropped.push_back(slots[i]);
        }
    }

    if(hover.itemCount <= 0)
        return dropped;

    moveIn(hover, altered);

    if(!hover.isEmpty()){
        dropped.push_back(hover);
    }

    return dropped;
}
