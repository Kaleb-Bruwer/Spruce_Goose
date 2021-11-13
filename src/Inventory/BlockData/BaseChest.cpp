#include "BaseChest.h"

#include <cstring>
#include <cmath>

#include "../Inventory2.h"

using namespace std;

BlockData* ChestSingle::clone(){
    BlockDataS* result = new ChestSingle();
    memcpy(result->slots, slots, sizeof(slots));
    return result;
}

BlockData* ChestDouble::clone(){
    BlockDataS* result = new ChestDouble();
    memcpy(result->slots, slots, sizeof(slots));
    return result;
}

ChestDouble::ChestDouble(ChestSingle& c1, ChestSingle& c2){
    memcpy(slots, c1.slots, sizeof(c1.slots));
    memcpy(slots + 27, c2.slots, sizeof(c2.slots));
}

void ChestDouble::splitChest(ChestSingle& c1, ChestSingle& c2){
    memcpy(c1.slots, slots, sizeof(c1.slots));
    memcpy(c2.slots, slots + 27, sizeof(c2.slots));
}

template <int nSlots>
vector<Slot> BaseChest<nSlots>::clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative){
    // 0 to nSlots-1 is the chest
    // nSlots to nSlots + 26 is the main inventory
    // nSlots + 27 to nSlots + 35 is the hotbar
    vector<Slot> dropped;

    const int invOffset = nSlots - 9;
    int clicked = job->slotNum;
    int btn = job->button;

    switch(job->mode){
    case 1: //Shift clicks
            // Validity checks
            if(btn != 0 && btn != 1)
                return dropped;
            if(clicked < 0 || clicked >= nSlots + 36)
                return dropped;

            if(clicked < nSlots){
                // Within chest
                altered.setOffset(invOffset);
                inv->mov(this->slots[clicked], 44, 9, altered);
                altered.setOffset(0);
                altered.add(clicked, this->slots[clicked]);
            }
            else{
                // Within inventory
                this->mov(inv->slots[clicked - invOffset], 0, nSlots - 1, altered);
                altered.add(clicked, inv->slots[clicked - invOffset]);
            }

        break;
    case 5: //mouse drags
        mouseDrag(job, inv, altered);

        break;
    case 6:{ //double click
        // Fill hover with similar items, starting from slots[0].
        // Take from partial stacks first, then full ones
        Slot& hover = inv->hover;

        if(hover.isEmpty() || clicked < 0 || clicked > nSlots + 35)
            break;

        short int stackLimit = hover.maxStackSize();

        // partial stacks
        for(int i = 0; i < nSlots; i++){
            if(hover.itemCount == stackLimit)
                break;

            if(this->slots[i].typeMatch(hover) && this->slots[i].itemCount < stackLimit){
                int take = min(stackLimit - hover.itemCount, (int) this->slots[i].itemCount);
                this->slots[i].itemCount -= take;
                hover.itemCount += take;

                if(this->slots[i].isEmpty())
                    this->slots[i].makeEmpty();
            }

        }
        if(hover.itemCount == stackLimit)
            break; //1st break only escaped the for loop
        for(int i = nSlots; i< nSlots + 36; i++){
            if(hover.itemCount == stackLimit)
                break;

            if(inv->slots[i- invOffset].typeMatch(hover) && this->slots[i].itemCount < stackLimit){
                int take = min(stackLimit - hover.itemCount, (int) this->slots[i- invOffset].itemCount);
                this->slots[i- invOffset].itemCount -= take;
                hover.itemCount += take;

                if(this->slots[i- invOffset].isEmpty())
                    this->slots[i- invOffset].makeEmpty();
            }
        }
        if(hover.itemCount == stackLimit)
            break;

        // Full stacks
        for(int i = 0; i < nSlots; i++){
            if(hover.itemCount == stackLimit)
                break;

            if(this->slots[i].typeMatch(hover)){
                int take = min(stackLimit - hover.itemCount, (int) this->slots[i].itemCount);
                this->slots[i].itemCount -= take;
                hover.itemCount += take;

                if(this->slots[i].isEmpty())
                    this->slots[i].makeEmpty();
            }

        }
        if(hover.itemCount == stackLimit)
            break; //1st break only escaped the for loop
        for(int i = nSlots; i< nSlots + 36; i++){
            if(hover.itemCount == stackLimit)
                break;

            if(inv->slots[i- invOffset].typeMatch(hover)){
                int take = min(stackLimit - hover.itemCount, (int) this->slots[i- invOffset].itemCount);
                this->slots[i- invOffset].itemCount -= take;
                hover.itemCount += take;

                if(this->slots[i- invOffset].isEmpty())
                    this->slots[i- invOffset].makeEmpty();
            }
        }

        break;
    }
    default:
        if(clicked < nSlots){
            Slot& origin = this->slots[clicked];

            // Within chest
            switch(job->mode){
            case 0: //Normal clicks
                // Validity check
                if(clicked < 0)
                    break;

                if(btn == 0){ //left click
                    if(!inv->hover.isEmpty() && inv->hover.typeMatch(origin)){
                        // Add to origin
                        int take = min(inv->hover.itemCount,
                            (short int) (origin.maxStackSize() - origin.itemCount));
                        inv->hover.itemCount -= take;
                        if(inv->hover.isEmpty())
                            inv->hover.makeEmpty();

                        origin.itemCount += take;
                        altered.add(clicked, origin);
                    }
                    else{
                        // Swap
                        Slot temp = origin;
                        origin = inv->hover;
                        inv->hover = temp;
                        altered.add(clicked, origin);
                    }
                }
                else if(btn == 1){ //right click
                    if(inv->hover.isEmpty()){ //pick up half
                        int take = ceil(origin.itemCount/2.0);
                        inv->hover = origin;
                        origin.itemCount -= take;
                        inv->hover.itemCount = take;

                        if(origin.isEmpty()) //if there was only 1 item
                            origin.makeEmpty();
                    }
                    else{ //place 1 item
                        if(origin.isEmpty() || (inv->hover.typeMatch(origin)
                                && origin.itemCount < origin.maxStackSize())){
                            inv->hover.itemCount--;

                            // Done this way to also cover empty origin case
                            int c = origin.itemCount;
                            origin = inv->hover;
                            origin.itemCount = c + 1;

                            if(inv->hover.isEmpty())
                                inv->hover.makeEmpty();
                        }
                        else if(!inv->hover.typeMatch(origin)){
                            // If mismatched, this is equivalent to a left-click
                            Slot temp = origin;
                            origin = inv->hover;
                            inv->hover = temp;
                        }
                    }
                }


                break;
            case 2:{ //Number keys
                // Validity checks
                if(btn < 0 || btn > 8)
                    break;
                if(clicked < 0)
                    break;
                if(!inv->hover.isEmpty())
                    break;


                Slot& target = inv->slots[36 + btn];
                if(target.isEmpty() || origin.isEmpty()){
                    Slot temp = target;
                    target = origin;
                    origin = temp;
                    altered.add(clicked, origin);
                    altered.add(36 + btn + invOffset, target);
                }
                else{
                    // Both contain stuff, 1.7's implementation of this case
                    // is weird and I need to match it...
                    // WHY DIDN'T THEY JUST SWAP IT? THE CORRECT IMPLEMENTATION
                    // IS SO MUCH EASIER THAN THIS MESS

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
                        break;

                    if(target.typeMatch(origin)){
                        // Same type
                        int take = target.maxStackSize() - target.itemCount;
                        take = min(take, (int) origin.itemCount);

                        origin.itemCount -= take;
                        target.itemCount += take;
                        altered.add(36 + btn + invOffset, target);

                        if(origin.isEmpty())
                            origin.makeEmpty();
                        else{
                            // Place what's left of origin on some open slot
                            // First check hotbar
                            for(int i=36; i<45; i++){
                                if(inv->slots[i].isEmpty()){
                                    inv->slots[i] = origin;
                                    origin.makeEmpty();
                                    altered.add(invOffset + i, inv->slots[i]);
                                    break;
                                }
                            }
                            if(!origin.isEmpty()){
                                for(int i=9; i<36; i++){
                                    if(inv->slots[i].isEmpty()){
                                        inv->slots[i] = origin;
                                        origin.makeEmpty();
                                        altered.add(invOffset + i, inv->slots[i]);
                                        break;
                                    }
                                }
                            }
                        }
                        altered.add(clicked, origin);
                    }
                    else{
                        // Different types
                        // Move target out first

                        // Check for partial stacks
                        for(int i=36; i<45; i++){
                            if(i - 36 == btn) //Can't move to self
                                continue;
                            if(inv->slots[i].typeMatch(target)){
                                int take = target.maxStackSize() - inv->slots[i].itemCount;
                                take = min(take, (int) target.itemCount);

                                target.itemCount -= take;
                                inv->slots[i].itemCount += take;

                                altered.add(i + invOffset, inv->slots[i]);
                                if(target.isEmpty()){
                                    target.makeEmpty();
                                    break;
                                }
                            }
                        }
                        if(!target.isEmpty()){
                            for(int i=9; i<36; i++){
                                if(inv->slots[i].typeMatch(target)){
                                    int take = target.maxStackSize() - inv->slots[i].itemCount;
                                    take = min(take, (int) target.itemCount);

                                    target.itemCount -= take;
                                    inv->slots[i].itemCount += take;

                                    altered.add(i + invOffset, inv->slots[i]);
                                    if(target.isEmpty()){
                                        target.makeEmpty();
                                        break;
                                    }
                                }
                            }
                        }

                        // Look for an open slot
                        if(!target.isEmpty()){
                            for(int i=36; i<45; i++){
                                if(inv->slots[i].isEmpty()){
                                    inv->slots[i] = target;
                                    target.makeEmpty();
                                    altered.add(i + invOffset, inv->slots[i]);
                                    break;
                                }
                            }

                            if(!target.isEmpty()){
                                for(int i=9; i<36; i++){
                                    if(inv->slots[i].isEmpty()){
                                        inv->slots[i] = target;
                                        target.makeEmpty();
                                        altered.add(i + invOffset, inv->slots[i]);
                                        break;
                                    }
                                }
                            }

                        }

                        // Now move origin to target
                        target = origin;
                        origin.makeEmpty();
                        altered.add(clicked, origin);
                        altered.add(36 + btn + invOffset, target);
                    }
                }
                break;
            }
            case 3: //middle click (creative only)
                // Validity check
                if(clicked < 0 || clicked > nSlots + 35)
                    break;

                if(creative && inv->hover.isEmpty() && btn == 2){
                    inv->hover = origin;
                    inv->hover.itemCount = inv->hover.maxStackSize();
                }

                break;
            case 4: //Drop item(s)
                if(clicked < 0 || origin.isEmpty())
                    break;
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
        else{
            // Within inventory
            altered.setOffset(invOffset);
            job->slotNum -= invOffset;
            dropped = inv->clickWindow(job, inv, altered, creative);
            altered.setOffset(0);
        }
        break;
    }


    return dropped;
}

template <int nSlots>
void BaseChest<nSlots>::mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered){
    // use DragData in inventory, since this info must be player specific
    // putting it in CraftingTable would make it shared between players
    const int invOffset = nSlots - 9;

    int clicked = job->slotNum;
    int btn = job->button;

    // Validity check
    if((btn == 1 || btn == 5) && (clicked < 0 || clicked > 45))
        return;

    switch(btn){
        case 0: //start left drag
            inv->dragData.dragMode = LEFT;
            inv->dragData.dragTotal = inv->hover.itemCount;
            break;

        case 1: //add slot to left drag
        // First check some conditions
        if(clicked < 0 || clicked > nSlots + 35) //invalid slot
            break;
        if(inv->dragData.dragMode == LEFT){
            Slot clickedSlot;
            if(clicked < nSlots)
                clickedSlot = this->slots[clicked];
            else
                clickedSlot = inv->slots[clicked - invOffset];

            if(!clickedSlot.isEmpty() && !clickedSlot.typeMatch(inv->hover))
                break;

            inv->dragData.dragSlots.push_back(clicked);
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

            for(int j=0; j<inv->dragData.dragSlots.size(); j++){
                int s = inv->dragData.dragSlots[j];

                Slot* slot;
                if(s < nSlots)
                    slot = &(this->slots[s]);
                else
                    slot = &inv->slots[s - invOffset];

                *slot = inv->hover; // sets the type
                slot->itemCount = inv->dragData.baseCount[j] + numToAdd;
                if(slot->itemCount > maxStack){
                    remainder += slot->itemCount - maxStack;
                    this->slots->itemCount = maxStack;
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
                if(s < nSlots)
                    this->slots[s] = inv->hover;
                else
                    inv->slots[s - invOffset] = inv->hover;

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
        if(clicked < 0 || clicked > nSlots + 35) //invalid
            break;
        if(inv->dragData.dragMode == RIGHT){
            Slot *slot;
            if(clicked < nSlots)
                slot = &(this->slots[clicked]);
            else
                slot = &inv->slots[clicked - invOffset];

            if(!slot->isEmpty() && !slot->typeMatch(inv->hover))
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
            altered.add(clicked, *slot);
        }
        break;

        case 6: //end right drag
            inv->dragData.dragMode = NONE;
        break;
    }
}
