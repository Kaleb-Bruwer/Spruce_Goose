#include "CraftingTable.h"

#include <iostream>
#include <climits>
#include <cstring>
#include <cmath>

#include "../Inventory2.h"
#include "../Crafting/Crafting.h"
#include "../Crafting/CraftingFrame.h"

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
        int lowestLeft = INT_MAX;
        for(int i=1; i<10; i++){
            int temp = slots[i].itemCount;
            if(temp > 0 && temp < lowestLeft)
                lowestLeft = temp;
        }
        if(lowestLeft == INT_MAX)
            return;

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

    CraftingFrame fr(3,3);
    fr.frame[0][0] = slots[1];
    fr.frame[0][1] = slots[2];
    fr.frame[0][2] = slots[3];

    fr.frame[1][0] = slots[4];
    fr.frame[1][1] = slots[5];
    fr.frame[1][2] = slots[6];

    fr.frame[2][0] = slots[7];
    fr.frame[2][1] = slots[8];
    fr.frame[2][2] = slots[9];

    slots[0] = Crafting::getInstance()->getProduct(fr);
    altered.add(0, slots[0]);
}

vector<Slot> CraftingTable::close(InventoryControl* invCon, AlteredSlots& altered, Inventory2* inv){
    BlockData::close(invCon, altered, inv);
    // TODO: clear crafting frame

    vector<Slot> dropped;
    for(int i=1; i<10; i++){
        inv->moveIn(slots[i], altered);
        if(!slots[i].isEmpty()){
            dropped.push_back(slots[i]);
            slots[i].makeEmpty();
        }
    }

    return dropped;
}


// void CraftingTable::clickWindow(ClickWindowJob* job, Inventory* inv, bool creative){
vector<Slot> CraftingTable::clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative){
    vector<Slot> dropped;

    int clicked = job->slotNum;
    Slot& hover = inv->hover;

    if(clicked < 0){
        return dropped; //invalid request
    }

    if(job->mode == 5){
        // inventory or not, these need to be handled in the same place
        mouseDrag(job, inv, altered);
        return dropped;
    }
    if(job->mode == 6){
        //double click
        int i = clicked;

        if(i <= 0 || i > 45) //invalid
            return dropped;

        int stackLimit = hover.maxStackSize();
        // Inside inventory, not armour and not crafting result

        if(inv->hover.isEmpty())
            return dropped;
        // Fill from incomplete stacks
        for(int j=1; j < 10; j++){ //crafting frame
            if(slots[j].typeMatch(inv->hover) && slots[j].itemCount < stackLimit){
                tryInsertHalfSlot(inv->hover, slots[j], stackLimit);
                altered.add(j, slots[j]);
                if(inv->hover.itemCount == stackLimit)
                    return dropped;
            }
        }
        for(int j=9; j < 45; j++){ //inventory
            if(inv->slots[j].typeMatch(inv->hover) && inv->slots[j].itemCount < stackLimit){
                tryInsertHalfSlot(inv->hover, inv->slots[j], stackLimit);
                altered.add(j, inv->slots[j]);
                if(inv->hover.itemCount == stackLimit)
                    return dropped;
            }
        }

        // Fill from full stacks
        for(int j=1; j < 10; j++){ //crafting table
            if(slots[j].typeMatch(inv->hover)){
                tryInsertHalfSlot(inv->hover, slots[j], stackLimit);
                altered.add(j, slots[j]);
                if(inv->hover.itemCount == stackLimit)
                    return dropped;
            }
        }
        for(int j=9; j < 45; j++){ //inventory
            if(inv->slots[j].typeMatch(inv->hover)){
                tryInsertHalfSlot(inv->hover, inv->slots[j], stackLimit);
                altered.add(j, inv->slots[j]);
                if(inv->hover.itemCount == stackLimit)
                    return dropped;
            }
        }

        return dropped;
    }

    if(clicked < 10){
        // Action involves crafting window
        Slot& origin = slots[clicked];

        switch(job->mode){
        case 0:
            //normal click
            if(clicked == 0){
                // crafting output
                // TODO: crafting
            }
            else{
                // crafting frame
                if(job->button == 0){
                    // Left click
                    Slot temp = hover;
                    hover = origin;
                    origin = temp;
                    altered.add(clicked, origin);
                }
                else if(job->button == 1){
                    if(hover.isEmpty()){
                        int take = ceil(origin.itemCount/2.0);
                        hover = origin;
                        origin.itemCount -= take;
                        hover.itemCount = take;

                        if(origin.isEmpty())
                            origin.makeEmpty();
                    }
                    else{
                        if(origin.isEmpty() || (hover.typeMatch(origin)
                                && origin.itemCount < origin.maxStackSize())){
                            hover.itemCount--;

                            // Done this way to also cover empty origin case
                            int c = origin.itemCount;
                            origin = hover;
                            origin.itemCount = c + 1;

                            if(hover.isEmpty())
                                hover.makeEmpty();
                        }
                        else if(!hover.typeMatch(origin)){
                            Slot temp = origin;
                            origin = hover;
                            hover = temp;
                        }
                    }
                }
                else if(job->button == 3){
                    //create new stack, creative mode only
                    if(creative && !origin.isEmpty()){
                        hover = origin;
                        hover.itemCount = hover.maxStackSize();
                    }
                }
                altered.add(clicked, origin);
            }
            break;
        case 1:
            //shift click
            if(job->button != 0 && job->button != 1) //invalid request
                return dropped;

            if(clicked==0){
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
            if(job->button < 0 || job->button > 8)
                return dropped; //invalid request

            if(clicked != 0){
                Slot& target = inv->slots[36 + job->button];
                Slot temp = target;
                target = origin;
                origin = temp;
            }
            else{
                // crafting product (TODO)
            }
            break;
        case 3:
            //middle click
            if(creative && hover.isEmpty() && job->button == 2){
                hover = slots[clicked];
                if(!hover.isEmpty())
                    hover.itemCount = hover.maxStackSize();
            }
            break;
        case 4:
        // Buttons 0 & 1 with i = -999 are in protocol, but as no-op
            if(job->button == 0){
                //Drop one
                Slot drop = slots[clicked];
                drop.itemCount = 1;
                slots[clicked].itemCount--;
                if(slots[clicked].isEmpty())
                    slots[clicked].makeEmpty();

                dropped.push_back(drop);
                altered.add(clicked, slots[clicked]);
            }
            else if(job->button == 1){
                //Drop stack
                dropped.push_back(slots[clicked]);
                slots[clicked].makeEmpty();
                altered.add(clicked, slots[clicked]);
            }
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


void CraftingTable::mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered){
    // use DragData in inventory, since this info must be player specific
    // putting it in CraftingTable would make it shared between players
    int i = job->slotNum;
    if(i < 0 || i > 45)
        return;

    switch(job->button){
        case 0: //start left drag
            inv->dragData.dragMode = LEFT;
            inv->dragData.dragTotal = inv->hover.itemCount;
            break;

        case 1: //add slot to left drag
        // First check some conditions
        if(i <0 || i > 44) //invalid
            break;
        if(inv->dragData.dragMode == LEFT){
            Slot clickedSlot;
            if(i < 10)
                clickedSlot = slots[i];
            else
                clickedSlot = inv->slots[i-1];

            if(!clickedSlot.isEmpty() && !clickedSlot.typeMatch(inv->hover))
                break;

            inv->dragData.dragSlots.push_back(i);
            inv->dragData.baseCount.push_back(clickedSlot.itemCount);

            // Conditions where no action is required
            int numSlotsInDrag = inv->dragData.dragSlots.size();
            if(numSlotsInDrag <= 1 || numSlotsInDrag > inv->dragData.dragTotal)
                break;

            // Split from scrach every time
            // numToAdd: num added to each slot in dragSlots
            int numToAdd = floor((float)inv->dragData.dragTotal/numSlotsInDrag);
            int remainder = inv->dragData.dragTotal - numToAdd * numSlotsInDrag;

            int maxStack = inv->hover.maxStackSize();

            for(int i=0; i<inv->dragData.dragSlots.size(); i++){
                int s = inv->dragData.dragSlots[i];

                Slot* slot;
                if(s < 10)
                    slot = &slots[s];
                else
                    slot = &inv->slots[s-1];

                *slot = inv->hover; // sets the type
                slot->itemCount = inv->dragData.baseCount[i] + numToAdd;
                if(slot->itemCount > maxStack){
                    remainder += slot->itemCount - maxStack;
                    slots->itemCount = maxStack;
                }
                altered.add(s, *slot);
            }
            inv->hover.itemCount = remainder;
        }
        break;

        case 2: //end left drag
            inv->dragData.dragMode = NONE;

            if(inv->dragData.dragSlots.size() == 1){
                int s = inv->dragData.dragSlots[0];
                if(s < 10)
                    slots[s] = inv->hover;
                else
                    inv->slots[s-1] = inv->hover;

                inv->hover.makeEmpty();
            }
            if(inv->hover.isEmpty())
                inv->hover.makeEmpty();

            inv->dragData.dragSlots.clear();
            inv->dragData.baseCount.clear();
            break;


        case 4: //start right drag
            inv->dragData.dragMode = RIGHT;
            inv->dragData.dragTotal = inv->hover.itemCount;
        break;

        case 5: //add slot to right drag
        if(i <0 || i > 44) //invalid
            break;
        if(inv->dragData.dragMode == RIGHT){
            Slot *slot;
            if(i < 10)
                slot = &slots[i];
            else
                slot = &inv->slots[i-1];

            if(!slot->isEmpty() && !slots->typeMatch(inv->hover))
                break;

            // Conditions where no action is required
            if(inv->hover.itemCount == 0)
                break;

            // Add one item to new slot (if possible)
            int maxStack = inv->hover.maxStackSize();
            if(slot->itemCount < maxStack){
                slot->itemID = inv->hover.itemID;
                slot->itemDamage = inv->hover.itemDamage;
                slot->itemCount++;
                inv->hover.itemCount--;
                if(inv->hover.itemCount == 0)
                    inv->hover.makeEmpty();
            }
            altered.add(i, *slot);
        }
        break;

        case 6: //end right drag

            inv->dragData.dragMode = NONE;
        break;
    }
}
