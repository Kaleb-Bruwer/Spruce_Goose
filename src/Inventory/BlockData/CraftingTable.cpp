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

    if(clicked < 10){
        // Action involves crafting window
        Slot& origin = slots[clicked];

        if(job->shift){
            if(job->button == 3)
                return dropped;

            if(clicked == 0){
                //Crafting output
                craft(true, altered);

                // altered.setOffset(0); Offset is redundant, it happens to be 0
                inv->mov(origin, 44, 9, altered);
                altered.add(0, slots[0]);
                checkCrafting(altered);
            }
            else{
                //Crafting frame
                inv->mov(origin, 10, 45, altered);
                altered.add(clicked, slots[clicked]);
                checkCrafting(altered);
            }
        }
        else{
            //Not shifted, regular click

            //Making hover a reference here might cause problems with
            //assignment operator
            Slot& hover = inv->hover;

            if(!hover.isEmpty()){
                //Placing something in crafting bench
                if(clicked == 0)
                    return dropped; //Can't place anything in product frame

                if(job->button == 0){
                    if(origin.isEmpty()){
                        //Put down in empty slot
                        origin = hover;
                        hover.makeEmpty();
                        checkCrafting(altered);
                        altered.add(clicked, slots[clicked]);
                    }
                    else{
                        //Attempt to put down in non-empty slot
                        if(origin.typeMatch(hover)){
                            int canMove = origin.maxStackSize() - origin.itemCount;
                            canMove = min(canMove, (int) hover.itemCount);
                            origin.itemCount += canMove;
                            hover.itemCount -= canMove;
                            if(hover.isEmpty())
                                hover.makeEmpty();
                            checkCrafting(altered);
                            altered.add(clicked, slots[clicked]);
                        }
                    }
                }
                else if(job->button == 1){
                    if(origin.isEmpty()){
                        origin = hover;
                        origin.itemCount = 1;
                        hover.itemCount--;
                        if(hover.isEmpty()){
                            hover.makeEmpty();
                        }
                        checkCrafting(altered);
                        altered.add(clicked, slots[clicked]);
                    }
                    else{
                        if(origin.typeMatch(hover)){
                            if(origin.itemCount < origin.maxStackSize()){
                                origin.itemCount++;
                                hover.itemCount--;
                                if(hover.isEmpty()){
                                    hover.makeEmpty();
                                }
                                checkCrafting(altered);
                                altered.add(clicked, slots[clicked]);
                            }
                        }
                    }
                }
            }
            else{
                //Hover is empty
                if(clicked == 0){
                    //Crafting result
                    if(job->button == 3)
                        return dropped;

                    craft(false, altered);
                    hover = origin;
                    origin.makeEmpty();
                    checkCrafting(altered);
                }
                else{
                    //Crafting frame
                    switch(job->button){
                    case 0:
                        hover = origin;
                        origin.makeEmpty();
                        altered.add(clicked, slots[clicked]);
                        break;
                    case 1:
                    {
                        int take = ceil((double) origin.itemCount/2);
                        hover = origin;
                        hover.itemCount= take;
                        origin.itemCount -= take;
                        if(origin.isEmpty())
                            origin.makeEmpty();
                        altered.add(clicked, slots[clicked]);
                        break;
                    }
                    case 3:
                        if(creative && !origin.isEmpty()){
                            hover = origin;
                            hover.itemCount = hover.maxStackSize();
                            altered.add(clicked, slots[clicked]);
                        }
                    }
                }
            }
        }
    }
    else{
        //Clicked in inventory
        //Simply call Inventory2::clickInventory, since this case is exactly the
        //same as that
        //In future versions of MC this changes

        job->slotNum--;
        return inv->clickWindow(job, inv, altered, creative);
    }

    return dropped;
}
