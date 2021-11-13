#pragma once

#include<gtest/gtest.h>

#include "../../src/Inventory/Inventory2.h"
#include "../../src/Inventory/BlockData/BaseChest.h"
#include "../../src/Inventory/BlockData/CraftingTable.h"


void validateInventory(Inventory2& inventory2, vector<int> pos, vector<Slot> items, Slot hover){
    ASSERT_TRUE(pos.size() == items.size()) << "Test config invalid";

    // First sort input
    if(pos.size() > 1){
        for(int i=0; i<pos.size() -1; i++){
            short smallest = pos[i];
            short index = i;

            for(int j=i+1; j<pos.size(); j++){
                if(pos[j] < smallest){
                    smallest = pos[j];
                    index = j;
                }
            }
            int temp = pos[i];
            pos[i] = pos[index];
            pos[index] = temp;

            Slot tempS = items[i];
            items[i] = items[index];
            items[index] = tempS;
        }
    }

    int specialPos = 0;
    for(int i=0; i<inventory2.numSlots; i++){
        if(specialPos < pos.size() && pos[specialPos] == i){
            ASSERT_TRUE(inventory2.slots[i] == items[specialPos])
                << "[" << i << "]: " << inventory2.slots[i] << ", expected: " << items[specialPos];
            specialPos++;
        }
        else{
            ASSERT_TRUE(inventory2.slots[i].isEmpty())
                << "[" << i << "]: " << inventory2.slots[i] << ", expected: empty";
        }
    }

    ASSERT_TRUE(inventory2.hover == hover)
        << "[hover]: " << inventory2.hover << ", expected: " << hover;

}

void validateInventory(Inventory2& lhs, Inventory2& rhs){
    for(int i=0; i<45; i++){
        ASSERT_TRUE(lhs.slots[i] == rhs.slots[i])
            << "[" << i << "]: " << lhs.slots[i] << ", expected: " << rhs.slots[i];
    }
    ASSERT_TRUE(lhs.hover == rhs.hover)
        << "[hover]: " << lhs.hover << ", expected: " << rhs.hover;
}

void validateChestSingle(ChestSingle& c, vector<int> pos, vector<Slot> items){
    ASSERT_TRUE(pos.size() == items.size()) << "Test config invalid";

    // First sort input
    if(pos.size() > 1){
        for(int i=0; i<pos.size() -1; i++){
            short smallest = pos[i];
            short index = i;

            for(int j=i+1; j<pos.size(); j++){
                if(pos[j] < smallest){
                    smallest = pos[j];
                    index = j;
                }
            }
            int temp = pos[i];
            pos[i] = pos[index];
            pos[index] = temp;

            Slot tempS = items[i];
            items[i] = items[index];
            items[index] = tempS;
        }
    }

    int specialPos = 0;
    for(int i=0; i<27; i++){
        if(specialPos < pos.size() && pos[specialPos] == i){
            ASSERT_TRUE(c.slots[i] == items[specialPos])
                << "[" << i << "]: " << c.slots[i] << ", expected: " << items[specialPos];
            specialPos++;
        }
        else{
            ASSERT_TRUE(c.slots[i].isEmpty())
                << "[" << i << "]: " << c.slots[i] << ", expected: empty";
        }
    }
}
