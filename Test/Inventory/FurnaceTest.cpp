#include<gtest/gtest.h>

#include "../../src/Inventory/BlockData/Furnace.h"
#include "../../src/Inventory/Crafting/Crafting.h"

class FurnaceTest : public ::testing::Test{
protected:

    void testMode0Btn0(Slot s, Slot h, int slot){
        Furnace furnace;
        Inventory2 inventory;

        if(slot < 3)
            furnace.slots[slot] = s;
        else
            inventory.slots[slot + 6] = s; //Inventory-Offset for a single chest is 18
        inventory.hover = h;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 0;
        job->button = 0;
        job->slotNum = slot;

        Slot sExpect = h;
        Slot hExpect = s;

        if(!s.isEmpty() && s.typeMatch(h)){
            sExpect.itemCount = s.itemCount + h.itemCount;

            int m = sExpect.maxStackSize();
            int leftover = sExpect.itemCount - m;

            if(leftover > 0){
                sExpect.itemCount = m;
                hExpect.itemCount = leftover;
            }
            else
                hExpect.makeEmpty();
        }

        AlteredSlots altered;
        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;

        furnace.clickWindow(request);
        if(slot < 3){
            validateFurnace(furnace, vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(inventory, vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(inventory, vector<int>{slot + 6}, vector<Slot>{sExpect}, hExpect);
            validateFurnace(furnace, vector<int>(), vector<Slot>());
        }

        delete job;
    }

    void testMode0Btn1(Slot s, Slot h, int slot){
        Furnace furnace;
        Inventory2 inventory;

        if(slot < 3)
            furnace.slots[slot] = s;
        else
            inventory.slots[slot + 6] = s; //Inventory-Offset for a single chest is 18
        inventory.hover = h;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 0;
        job->button = 1;
        job->slotNum = slot;

        AlteredSlots altered;
        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;

        furnace.clickWindow(request);

        Slot sExpect;
        Slot hExpect;
        // If hover is empty, pick up half of slot with hover getting +1 on odds

        if(h.isEmpty()){
            int items = ceil( ((float)s.itemCount) / 2);
            hExpect = s;
            sExpect = s;

            hExpect.itemCount = items;
            sExpect.itemCount -= items;

            if(sExpect.itemCount == 0){
                sExpect = Slot();
            }
        }
        else if(s.isEmpty() || (s.itemID == h.itemID)){
            if(s.isEmpty() || s.itemCount < s.maxStackSize()){
                sExpect = s;
                sExpect.itemID = h.itemID;
                sExpect.itemDamage = h.itemDamage;

                sExpect.itemCount++;
                hExpect = h;
                hExpect.itemCount--;
                if(hExpect.itemCount == 0)
                    hExpect = Slot();
            }
            else{ //Slot is full, can't add anything
                sExpect = s;
                hExpect = h;
            }
        }
        else{ //types don't match
            sExpect = h;
            hExpect = s;
        }

        if(slot < 3){
            validateFurnace(furnace, vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(inventory, vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(inventory, vector<int>{slot + 6}, vector<Slot>{sExpect}, hExpect);
            validateFurnace(furnace, vector<int>(), vector<Slot>());
        }
        delete job;
    }
};

TEST_F(FurnaceTest, mode0Btn0){
    vector<int> slotsToTest{0,1,3,29,30,38};

    Slot stone(1);
    Slot wood(5);
    Slot stoneStack = stone;
    stoneStack.itemCount = 64;

    for(int slot : slotsToTest){
        testMode0Btn0(Slot(), stone, slot);
        testMode0Btn0(stone, Slot(), slot);
        testMode0Btn0(stone, stone, slot);
        testMode0Btn0(stoneStack, stone, slot);
        testMode0Btn0(wood, stone, slot);
    }
}

TEST_F(FurnaceTest, mode0Btn1){
    vector<int> slotsToTest{0,1,3,29,30,38};

    Slot stone(1);
    Slot wood(5);
    Slot stoneStack = stone;
    stoneStack.itemCount = 64;

    for(int slot : slotsToTest){
        testMode0Btn1(Slot(), stone, slot);
        testMode0Btn1(stone, Slot(), slot);
        testMode0Btn1(stone, stone, slot);
        testMode0Btn1(stoneStack, stone, slot);
        testMode0Btn1(wood, stone, slot);
    }
}
