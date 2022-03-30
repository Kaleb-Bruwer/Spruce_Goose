#include<gtest/gtest.h>
#include <cmath>

#include "CommonHelpers.h"

#include "../../src/Inventory/BlockData/BaseChest.h"
#include "../../src/Inventory/Inventory2.h"

using namespace std;

class BaseChestTest : public ::testing::Test{
protected:
    AlteredSlots altered;


    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

    void testMode0Btn0(Slot s, Slot h, int slot){
        ChestSingle chest;
        Inventory2 inventory;

        if(slot < 27)
            chest.slots[slot] = s;
        else
            inventory.slots[slot-18] = s; //Inventory-Offset for a single chest is 18
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

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;

        chest.clickWindow(request);

        if(slot < 27){
            validateChestSingle(chest, vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(inventory, vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(inventory, vector<int>{slot - 18}, vector<Slot>{sExpect}, hExpect);
            validateChestSingle(chest, vector<int>(), vector<Slot>());
        }


        delete job;
    }

    void testMode0Btn1(Slot s, Slot h, int slot){
        ChestSingle chest;
        Inventory2 inventory;

        if(slot < 27)
            chest.slots[slot] = s;
        else
            inventory.slots[slot-18] = s; //Inventory-Offset for a single chest is 18
        inventory.hover = h;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 0;
        job->button = 1;
        job->slotNum = slot;

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;

        chest.clickWindow(request);

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

        if(slot < 27){
            validateChestSingle(chest, vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(inventory, vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(inventory, vector<int>{slot - 18}, vector<Slot>{sExpect}, hExpect);
            validateChestSingle(chest, vector<int>(), vector<Slot>());
        }
        delete job;
    }

    // Inventory and chest added as parameters
    void testMode1(Slot s, Slot h, int slot, Inventory2 inventory = Inventory2(),
            ChestSingle chest = ChestSingle()){

        if(slot < 27)
            chest.slots[slot] = s;
        else
            inventory.slots[slot-18] = s; //Inventory-Offset for a single chest is 18
        inventory.hover = h;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 1;
        job->button = 0;
        job->slotNum = slot;

        Slot hExpect = h;

        vector<int> chestSlots;
        vector<Slot> chestItems;

        vector<int> invSlots;
        vector<Slot> invItems;

        // The following logic was NOT copied from the implementation since that
        // would defeat the purpose of testing
        if(s.isEmpty()){
            // Nothing moved

            for(int i=0; i<27; i++){
                if(!chest.slots[i].isEmpty()){
                    chestSlots.push_back(i);
                    chestItems.push_back(chest.slots[i]);
                }
            }

            for(int i=9; i<45; i++){
                if(!inventory.slots[i].isEmpty()){
                    invSlots.push_back(i);
                    invItems.push_back(inventory.slots[i]);
                }
            }

        }
        else{
            // Something moved
            int stackSize = s.maxStackSize();

            if(slot < 27){
                // From chest to inventory
                int available = 0;
                for(int i=9; i<45; i++){
                    if(inventory.slots[i].isEmpty()){
                        available = stackSize;
                        break;
                    }
                    if(inventory.slots[i].typeMatch(s)){
                        available += stackSize - inventory.slots[i].itemCount;
                        if(available >= stackSize){
                            available = stackSize;
                            break;
                        }
                    }
                }

                available = min(available, (int) s.itemCount);

                // Available now specifies how many items from s will be moved
                Slot sExpect = s;
                sExpect.itemCount -= available;
                if(sExpect.isEmpty()){
                    sExpect.makeEmpty();
                }
                chestSlots.push_back(slot);
                chestItems.push_back(sExpect);

                // Look for partial stacks
                for(int i=44; i>=9; i--){
                    if(inventory.slots[i].typeMatch(s)){
                        if(available > 0){
                            int take = stackSize - inventory.slots[i].itemCount;
                            take = min(available, take);

                            Slot temp = inventory.slots[i];
                            temp.itemCount += take;
                            invSlots.push_back(i);
                            invItems.push_back(temp);

                            available -= take;
                        }
                        else{
                            invSlots.push_back(i);
                            invItems.push_back(inventory.slots[i]);
                        }
                    }
                }

                for(int i=44; i>=9; i--){
                    if(inventory.slots[i].isEmpty()){
                        if(available > 0){
                            Slot temp = s;
                            temp.itemCount = available;
                            invSlots.push_back(i);
                            invItems.push_back(temp);
                            available = 0;
                        }
                    }
                    else if(!inventory.slots[i].typeMatch(s)){
                        invSlots.push_back(i);
                        invItems.push_back(inventory.slots[i]);
                    }
                }
            }
            else{
                //From inventory to chest
                int available = 0;
                for(int i=0; i<27; i++){
                    if(chest.slots[i].isEmpty()){
                        available = stackSize;
                        break;
                    }
                    if(chest.slots[i].typeMatch(s)){
                        available += stackSize - chest.slots[i].itemCount;
                        if(available >= stackSize){
                            available = stackSize;
                            break;
                        }
                    }
                }

                available = min(available, (int) s.itemCount);

                // Available now specifies how many items from s will be moved
                Slot sExpect = s;
                sExpect.itemCount -= available;
                if(sExpect.isEmpty()){
                    sExpect.makeEmpty();
                }
                invSlots.push_back(slot - 18);
                invItems.push_back(sExpect);

                // Look for partial stacks
                for(int i=0; i<27; i++){
                    if(chest.slots[i].typeMatch(s)){
                        if(available > 0){
                            int take = stackSize - chest.slots[i].itemCount;
                            take = min(available, take);

                            Slot temp = chest.slots[i];
                            temp.itemCount += take;
                            chestSlots.push_back(i);
                            chestItems.push_back(temp);

                            available -= take;
                        }
                        else{
                            chestSlots.push_back(i);
                            chestItems.push_back(chest.slots[i]);
                        }
                    }
                }

                for(int i=0; i<27; i++){
                    if(chest.slots[i].isEmpty()){
                        if(available > 0){
                            Slot temp = s;
                            temp.itemCount = available;
                            chestSlots.push_back(i);
                            chestItems.push_back(temp);
                            available = 0;
                        }
                    }
                    else if(!chest.slots[i].typeMatch(s)){
                        chestSlots.push_back(i);
                        chestItems.push_back(chest.slots[i]);
                    }
                }
            }
        }

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;

        chest.clickWindow(request);

        validateInventory(inventory, invSlots, invItems, hExpect);
        validateChestSingle(chest, chestSlots, chestItems);

    }

    void testMode2(int slot, int btn, Inventory2 inv, ChestSingle chest){
        Slot hExpect = inv.hover;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 2;
        job->slotNum = slot;
        job->button = btn;

        vector<int> chestSlots;
        vector<Slot> chestItems;

        vector<int> invSlots;
        vector<Slot> invItems;

        if(!inv.hover.isEmpty()){
            for(int i=0; i<27; i++){
                if(!chest.slots[i].isEmpty()){
                    chestSlots.push_back(i);
                    chestItems.push_back(chest.slots[i]);
                }
            }
            for(int i=9; i<45; i++){
                if(!inv.slots[i].isEmpty()){
                    invSlots.push_back(i);
                    invItems.push_back(inv.slots[i]);
                }
            }
        }
        else if(slot >= 27){
            // Inventory <-> hotbar, just a swap
            for(int i=0; i<27; i++){
                if(!chest.slots[i].isEmpty()){
                    chestSlots.push_back(i);
                    chestItems.push_back(chest.slots[i]);
                }
            }
            for(int i=9; i<45; i++){
                if(i == slot - 18){
                    // Origin
                    invSlots.push_back(i);
                    invItems.push_back(inv.slots[36 + btn]);
                }
                else if (i == btn + 36){
                    // Target (on hotbar)
                    invSlots.push_back(i);
                    invItems.push_back(inv.slots[slot - 18]);
                }
                else{
                    if(!inv.slots[i].isEmpty()){
                        invSlots.push_back(i);
                        invItems.push_back(inv.slots[i]);
                    }
                }
            }
        }
        else{
            // origin is in the chest
            // This is the weird one

            if(chest.slots[slot].isEmpty() || inv.slots[36 + btn].isEmpty()){
                // Simple case: just swap
                for(int i=0; i<27; i++){
                    if(i == slot){
                        chestSlots.push_back(i);
                        chestItems.push_back(inv.slots[36 + btn]);
                    }
                    else{
                        if(!chest.slots[i].isEmpty()){
                            chestSlots.push_back(i);
                            chestItems.push_back(chest.slots[i]);
                        }
                    }
                }

                for(int i=9; i<45; i++){
                    if(i == 36 + btn){
                        invSlots.push_back(i);
                        invItems.push_back(chest.slots[slot]);
                    }
                    else{
                        if(!inv.slots[i].isEmpty()){
                            invSlots.push_back(i);
                            invItems.push_back(inv.slots[i]);
                        }
                    }
                }
            }
            else{
                // Both sides contain something
                Slot& origin = chest.slots[slot];
                Slot& target = inv.slots[36 + btn];

                bool found = false;
                for(int i=9; i<45; i++){
                    if(inv.slots[i].isEmpty()){
                        found = true;
                        break;
                    }
                }
                if(!found){
                    // no op scenario
                    for(int i=0; i<27; i++){
                        if(!chest.slots[i].isEmpty()){
                            chestSlots.push_back(i);
                            chestItems.push_back(chest.slots[i]);
                        }
                    }
                    for(int i=9; i<45; i++){
                        if(!inv.slots[i].isEmpty()){
                            invSlots.push_back(i);
                            invItems.push_back(inv.slots[i]);
                        }
                    }

                }
                else{
                    if(origin.typeMatch(target)){
                        // target can go everywhere, making this a really annoying one

                        // Handled tracks which slots already have their expected
                        // values written into invSlots & invItems
                        vector<bool> handled(36, false);

                        int remaining = origin.itemCount;
                        int maxStack = target.maxStackSize();
                        int take = min(maxStack - target.itemCount,
                            remaining);

                        Slot temp = target;
                        temp.itemCount += take;
                        remaining -= take;

                        handled[27 + btn] = true;
                        invSlots.push_back(36 + btn);
                        invItems.push_back(temp);

                        for(int i=36; i<45 && remaining > 0; i++){
                            if(i - 36 != btn && inv.slots[i].typeMatch(origin)){
                                take = min(maxStack - inv.slots[i].itemCount, remaining);
                                temp = inv.slots[i];
                                temp.itemCount += take;
                                remaining -= take;

                                handled[i - 9] = true;
                                invSlots.push_back(i);
                                invItems.push_back(temp);
                            }
                        }
                        for(int i=9; i<36 && remaining > 0; i++){
                            if(inv.slots[i].typeMatch(origin)){
                                take = min(maxStack - inv.slots[i].itemCount, remaining);
                                temp = inv.slots[i];
                                temp.itemCount += take;
                                remaining -= take;

                                handled[i - 9] = true;
                                invSlots.push_back(i);
                                invItems.push_back(temp);
                            }
                        }

                        for(int i=36; i<45 && remaining > 0; i++){
                            if(inv.slots[i].isEmpty()){
                                temp = origin;
                                temp.itemCount = remaining;
                                remaining = 0;

                                handled[i - 9] = true;
                                invSlots.push_back(i);
                                invItems.push_back(temp);
                            }
                        }
                        for(int i=9; i<36 && remaining > 0; i++){
                            if(inv.slots[i].isEmpty()){
                                temp = origin;
                                temp.itemCount = remaining;
                                remaining = 0;

                                handled[i - 9] = true;
                                invSlots.push_back(i);
                                invItems.push_back(temp);
                            }
                        }

                        // Validation for unaltered slots & origin
                        for(int i=9; i<45; i++){
                            if(!handled[i-9] && !inv.slots[i].isEmpty()){
                                invSlots.push_back(i);
                                invItems.push_back(inv.slots[i]);
                            }
                        }
                        for(int i=0; i<27; i++){
                            if(i == slot){
                                chestSlots.push_back(i);
                                chestItems.push_back(Slot());
                            }
                            else if(!chest.slots[i].isEmpty()){
                                chestSlots.push_back(i);
                                chestItems.push_back(chest.slots[i]);
                            }
                        }

                    }
                    else{
                        // Types don't match
                        vector<bool> handled(36, false);

                        // The obvious parts first
                        handled[27 + btn];
                        invSlots.push_back(36 + btn);
                        invItems.push_back(origin);

                        chestSlots.push_back(slot);
                        chestItems.push_back(Slot());

                        int maxStack = target.maxStackSize();
                        int remaining = target.itemCount;

                        vector<int> order;
                        for(int i=36; i<45; i++)
                            order.push_back(i);
                        for(int i=9; i<36; i++)
                            order.push_back(i);

                        for(int i : order){
                            if(i-36 == btn)
                                continue;
                            if(inv.slots[i].typeMatch(target)){
                                int take = min(maxStack - inv.slots[i].itemCount, remaining);

                                Slot temp;
                                temp = inv.slots[i];
                                temp.itemCount += take;
                                remaining -= take;

                                invSlots.push_back(i);
                                invItems.push_back(temp);
                            }
                            if(remaining == 0)
                                break;
                        }
                        for(int i : order){
                            if(inv.slots[i].isEmpty()){
                                Slot temp = target;
                                temp.itemCount = remaining;
                                remaining = 0;

                                invSlots.push_back(i);
                                invItems.push_back(temp);
                                break;
                            }
                        }

                        for(int i=0; i<27; i++){
                            if(i != slot && !chest.slots[i].isEmpty()){
                                chestSlots.push_back(i);
                                chestItems.push_back(chest.slots[i]);
                            }
                        }
                        for(int i=36; i<45; i++){
                            if(!handled[i-9] && !inv.slots[i].isEmpty()){
                                invSlots.push_back(i);
                                invItems.push_back(inv.slots[i]);
                            }
                        }

                    }
                }//if found
            }

        }

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inv;
        request.altered = &altered;
        request.creative = false;

        chest.clickWindow(request);
        delete job;

        validateInventory(inv, invSlots, invItems, hExpect);
        validateChestSingle(chest, chestSlots, chestItems);
    }

    void testMode4(int slot, int btn, Inventory2 inv, ChestSingle chest){
        // Assumes slot is the only slot with items in it

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 4;
        job->slotNum = slot;
        job->button = btn;

        // Determine what is expected
        Slot dropExpect;
        Slot sExpect;
        Slot hExpect = inv.hover;

        if(slot >= 0 && inv.hover.isEmpty()){
            if(slot < 27){
                if(!chest.slots[slot].isEmpty()){
                    dropExpect = chest.slots[slot];
                    if(btn == 0){
                        dropExpect.itemCount = 1;
                        sExpect = chest.slots[slot];
                        sExpect.itemCount--;
                        if(sExpect.isEmpty())
                            sExpect.makeEmpty();
                    }
                }
            }
            else{
                if(!inv.slots[slot - 18].isEmpty()){
                    dropExpect = inv.slots[slot - 18];
                    if(btn == 0){
                        dropExpect.itemCount = 1;
                        sExpect = inv.slots[slot - 18];
                        sExpect.itemCount--;
                        if(sExpect.isEmpty())
                            sExpect.makeEmpty();
                    }
                }
            }
        }

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inv;
        request.altered = &altered;
        request.creative = false;

        vector<Slot> dropped = chest.clickWindow(request);
        delete job;

        // Assume there's only one drop
        if(dropExpect.isEmpty()){
            ASSERT_TRUE(dropped.size() == 0) << "Dropped " << dropped[0]
                << " instead of nothing\n";
        }
        else{
            ASSERT_TRUE(dropped.size() != 0) << "Dropped nothing instead of " << dropExpect << endl;
            ASSERT_TRUE(dropped[0] == dropExpect) << "dropped " << dropped[0]
                << " instead of " << dropExpect << endl;
        }

        if(slot <= 27){
            validateInventory(inv, vector<int>(), vector<Slot>(), hExpect);
            validateChestSingle(chest, vector<int>{slot}, vector<Slot>{sExpect});
        }
        else{
            validateInventory(inv, vector<int>{slot - 18}, vector<Slot>{sExpect}, hExpect);
            validateChestSingle(chest, vector<int>(), vector<Slot>());
        }

    }

    void sendMode5Jobs(Inventory2 &inv, ChestSingle &chest, vector<int> slots, bool mode){
        ClickWindowJob* job = new ClickWindowJob();
        AlteredSlots altered;

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inv;
        request.altered = &altered;
        request.creative = false;

        if(!mode){
            // Left mouse drag
            job->mode = 5;
            job->button = 0;
            job->slotNum = -999;
            chest.clickWindow(request);

            job->button = 1;
            for(int slot : slots){
                job->slotNum = slot;
                chest.clickWindow(request);
            }

            job->button = 2;
            job->slotNum = -999;
            chest.clickWindow(request);
        }
        else{
            // Right mouse drag
            job->mode = 5;
            job->button = 4;
            job->slotNum = -999;
            chest.clickWindow(request);

            job->button = 5;
            for(int slot : slots){
                job->slotNum = slot;
                chest.clickWindow(request);
            }

            job->button = 6;
            job->slotNum = -999;
            chest.clickWindow(request);
        }
        delete job;
    }

};

TEST_F(BaseChestTest, mode0Btn0){
    vector<int> slotsToTest{0,26,27,28,53,54,62};

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

TEST_F(BaseChestTest, mode0Btn1){
    vector<int> slotsToTest{0,26,27,28,53,54,62};

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

TEST_F(BaseChestTest, mode1Test1){
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    Slot stone(1);
    Slot wood(5);
    Slot stoneStack = stone;
    stoneStack.itemCount = 64;

    for(int slot : slotsToTest){
        testMode1(Slot(1), Slot(), slot);
        // Hover shouldn't affect shift-clicks
        testMode1(Slot(1), wood, slot);
    }

    Inventory2 filledInventory;

    // Check inventory filled with other items
    for(int i=9; i<45; i++){
        filledInventory.slots[i] = wood;
    }

    for(int slot : vector<int>{0,26}){
        testMode1(Slot(1), Slot(), slot, filledInventory);
    }

    // Check inventory filled with full stacks
    for(int i=9; i<45; i++){
        filledInventory.slots[i] = stoneStack;
    }

    for(int slot : vector<int>{0,26}){
        testMode1(Slot(1), Slot(), slot, filledInventory);
    }


    ChestSingle filledChest;

    // Check chest filled with other items
    for(int i=0; i<27; i++){
        filledChest.slots[i] = wood;
    }

    for(int slot : vector<int>{27,28,53,54,62}){
        testMode1(Slot(1), Slot(), slot, Inventory2(), filledChest);
    }

    // Check inventory filled with full stacks
    for(int i=0; i<27; i++){
        filledChest.slots[i] = stoneStack;
    }

    for(int slot : vector<int>{27,28,53,54,62}){
        testMode1(Slot(1), Slot(), slot, Inventory2(), filledChest);
    }
}

TEST_F(BaseChestTest, mode1Test2){
    Slot stone(1);

    Inventory2 inventory;
    inventory.slots[14] = stone;
    inventory.slots[14].itemCount = 12;

    testMode1(stone, Slot(), 26, inventory);

    inventory.slots[19] = stone;
    inventory.slots[19].itemCount = 64;

    testMode1(stone, Slot(), 26, inventory);
}

TEST_F(BaseChestTest, mode2TestEmpty){
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;
        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2TestHover){
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    for(int slot : slotsToTest){
        Inventory2 inv;
        inv.hover = Slot(2);
        ChestSingle chest;

        if(slot < 27)
            chest.slots[slot] = Slot(1);
        else
            inv.slots[slot - 18] = Slot(1);


        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test1){
    // Move item to open slot on hotbar
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;

        if(slot < 27)
            chest.slots[slot] = Slot(1);
        else
            inv.slots[slot - 18] = Slot(1);

        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test2){
    // Move item from hotbar to open slot
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;

        inv.slots[35 + 3] = Slot(1);
        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test3){
    // slot and button are the same slot
    Inventory2 inv;
    ChestSingle chest;

    testMode2(54, 1, inv, chest);

    inv.slots[36] = Slot(1);
    testMode2(54, 1, inv, chest);
}

TEST_F(BaseChestTest, mode2Test4){
    // same item both sides
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;

        inv.slots[35 + 3] = Slot(1);
        if(slot < 27){
            chest.slots[slot] = Slot(1);
        }
        else{
            inv.slots[slot-18] = Slot(1);
        }

        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test5){
    // different item in hotbar
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    Slot wood(5);

    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;

        inv.slots[35 + 3] = wood;
        if(slot < 27){
            chest.slots[slot] = Slot(1);
        }
        else{
            inv.slots[slot-18] = Slot(1);
        }

        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test6){
    // more than one stack if combined
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    Slot stone48 = Slot(1);
    stone48.itemCount = 48;


    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;

        inv.slots[35 + 3] = stone48;
        if(slot < 27){
            chest.slots[slot] = stone48;
        }
        else{
            inv.slots[slot-18] = stone48;
        }

        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test7){
    // more than one stack if combined,
    // plus partial stack elsewhere
    vector<int> slotsToTest{0,26,27,28,53,54,62};

    Slot stone48 = Slot(1);
    stone48.itemCount = 48;


    for(int slot : slotsToTest){
        Inventory2 inv;
        ChestSingle chest;

        inv.slots[12] = Slot(1);

        inv.slots[35 + 3] = stone48;
        if(slot < 27){
            chest.slots[slot] = stone48;
        }
        else{
            inv.slots[slot-18] = stone48;
        }

        testMode2(slot, 3, inv, chest);
    }
}

TEST_F(BaseChestTest, mode2Test8){
    // more than one stack if combined,
    // plus partial stack elsewhere
    vector<int> slotsToTest{0,26};
    Inventory2 fullInv;

    for(int i=9; i<45; i++){
        fullInv.slots[i] = Slot(2);
    }

    for(int slot : slotsToTest){
        ChestSingle chest;
        chest.slots[slot] = Slot(1);

        testMode2(slot, 3, fullInv, chest);
    }
}

TEST_F(BaseChestTest, mode3Test1){
    // Try to dupicate items in survival
    //Set up inventory
    ChestSingle chest;
    Inventory2 inv;

    chest.slots[9] = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 3;
    job->button = 2;
    job->slotNum = 9;

    AlteredSlots altered;
    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    validateChestSingle(chest, vector<int>{9}, vector<Slot>{Slot(1)});
    validateInventory(inv, vector<int>(), vector<Slot>(), Slot());
    delete job;
}

TEST_F(BaseChestTest, mode3Test2){
    // Try to dupicate items in survival
    //Set up inventory
    ChestSingle chest;
    Inventory2 inv;

    inv.slots[9] = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 3;
    job->button = 2;
    job->slotNum = 27;

    AlteredSlots altered;
    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    validateChestSingle(chest, vector<int>(), vector<Slot>());
    validateInventory(inv, vector<int>{9}, vector<Slot>{Slot(1)}, Slot());
    delete job;
}

TEST_F(BaseChestTest, mode3Test3){
    // Dupicate items in creative
    //Set up inventory
    ChestSingle chest;
    Inventory2 inv;

    chest.slots[9] = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 3;
    job->button = 2;
    job->slotNum = 9;

    AlteredSlots altered;
    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = true;

    chest.clickWindow(request);

    Slot stoneStack = Slot(1);
    stoneStack.itemCount = 64;

    validateChestSingle(chest, vector<int>{9}, vector<Slot>{Slot(1)});
    validateInventory(inv, vector<int>(), vector<Slot>(), stoneStack);
    delete job;
}

TEST_F(BaseChestTest, mode3Test4){
    // Dupicate items in creative
    //Set up inventory
    ChestSingle chest;
    Inventory2 inv;

    inv.slots[9] = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 3;
    job->button = 2;
    job->slotNum = 27;

    AlteredSlots altered;
    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = true;

    chest.clickWindow(request);

    Slot stoneStack = Slot(1);
    stoneStack.itemCount = 64;

    validateChestSingle(chest, vector<int>(), vector<Slot>());
    validateInventory(inv, vector<int>{9}, vector<Slot>{Slot(1)}, stoneStack);
    delete job;
}

TEST_F(BaseChestTest, mode4Test1){
    testMode4(-999, 0, Inventory2(), ChestSingle());
    testMode4(-999, 1, Inventory2(), ChestSingle());

    testMode4(4, 0, Inventory2(), ChestSingle());
    testMode4(4, 1, Inventory2(), ChestSingle());

    testMode4(34, 0, Inventory2(), ChestSingle());
    testMode4(34, 1, Inventory2(), ChestSingle());
}

TEST_F(BaseChestTest, mode4Test2){
    ChestSingle chest;
    chest.slots[4] = Slot(1);

    testMode4(4, 0, Inventory2(), chest);
    testMode4(4, 1, Inventory2(), chest);

    chest.slots[4].itemCount = 64;

    testMode4(4, 0, Inventory2(), chest);
    testMode4(4, 1, Inventory2(), chest);
}

TEST_F(BaseChestTest, mode4Test3){
    Inventory2 inv;
    inv.slots[12] = Slot(1);

    testMode4(30, 0, inv, ChestSingle());
    testMode4(30, 1, inv, ChestSingle());

    inv.slots[12].itemCount = 64;

    testMode4(30, 0, inv, ChestSingle());
    testMode4(30, 1, inv, ChestSingle());
}

TEST_F(BaseChestTest, mode5Test1){
    Inventory2 inv;
    ChestSingle chest;

    inv.hover = Slot(1);
    inv.hover.itemCount = 32;
    sendMode5Jobs(inv, chest, vector<int>{2, 3}, false);

    Slot stone16 = Slot(1);
    stone16.itemCount = 16;

    validateInventory(inv, vector<int>(), vector<Slot>(), Slot());
    validateChestSingle(chest, vector<int>{2,3}, vector<Slot>{stone16, stone16});
}

TEST_F(BaseChestTest, mode5Test2){
    Inventory2 inv;
    ChestSingle chest;

    inv.hover = Slot(1);
    inv.hover.itemCount = 32;
    sendMode5Jobs(inv, chest, vector<int>{2, 3}, true);

    Slot stone30 = Slot(1);
    stone30.itemCount = 30;

    validateInventory(inv, vector<int>(), vector<Slot>(), stone30);
    validateChestSingle(chest, vector<int>{2,3}, vector<Slot>{Slot(1), Slot(1)});
}


TEST_F(BaseChestTest, mode5Test3){
    Inventory2 inv;
    ChestSingle chest;

    inv.hover = Slot(1);
    inv.hover.itemCount = 64;
    sendMode5Jobs(inv, chest, vector<int>{2, 3, 4}, false);

    Slot stone21 = Slot(1);
    stone21.itemCount = 21;

    validateInventory(inv, vector<int>(), vector<Slot>(), Slot(1));
    validateChestSingle(chest, vector<int>{2, 3, 4}, vector<Slot>{stone21, stone21, stone21});
}

TEST_F(BaseChestTest, mode5Test4){
    Inventory2 inv;
    ChestSingle chest;

    inv.hover = Slot(1);
    inv.hover.itemCount = 64;
    sendMode5Jobs(inv, chest, vector<int>{2, 3, 40}, false);

    Slot stone21 = Slot(1);
    stone21.itemCount = 21;

    validateInventory(inv, vector<int>{22}, vector<Slot>{stone21}, Slot(1));
    validateChestSingle(chest, vector<int>{2,3}, vector<Slot>{stone21, stone21});
}

TEST_F(BaseChestTest, mode5Test5){
    Inventory2 inv;
    ChestSingle chest;

    inv.hover = Slot(1);
    inv.hover.itemCount = 64;
    sendMode5Jobs(inv, chest, vector<int>{2, 3, 4}, true);

    Slot stone61 = Slot(1);
    stone61.itemCount = 61;

    validateInventory(inv, vector<int>(), vector<Slot>(), stone61);
    validateChestSingle(chest, vector<int>{2, 3, 4}, vector<Slot>{Slot(1), Slot(1), Slot(1)});
}

TEST_F(BaseChestTest, mode5Test6){
    Inventory2 inv;
    ChestSingle chest;

    inv.hover = Slot(1);
    inv.hover.itemCount = 64;
    sendMode5Jobs(inv, chest, vector<int>{2, 3, 40}, true);

    Slot stone61 = Slot(1);
    stone61.itemCount = 61;

    validateInventory(inv, vector<int>{22}, vector<Slot>{Slot(1)}, stone61);
    validateChestSingle(chest, vector<int>{2,3}, vector<Slot>{Slot(1), Slot(1)});
}

TEST_F(BaseChestTest, mode6Test1){
    Inventory2 inv;
    ChestSingle chest;

    for(int i=9;i < 20; i++){
        inv.slots[i] = Slot(1);
    }
    inv.hover = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 6;
    job->button = 0;
    job->slotNum = 21;

    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    Slot stone12 = Slot(1);
    stone12.itemCount = 12;

    validateInventory(inv, vector<int>(), vector<Slot>(), stone12);
    validateChestSingle(chest, vector<int>(), vector<Slot>());

    delete job;
}

TEST_F(BaseChestTest, mode6Test2){
    Inventory2 inv;
    ChestSingle chest;

    for(int i=0;i < 10; i++){
        chest.slots[i] = Slot(1);
    }

    for(int i=9;i < 20; i++){
        inv.slots[i] = Slot(1);
    }
    inv.hover = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 6;
    job->button = 0;
    job->slotNum = 21;

    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    Slot stone = Slot(1);
    stone.itemCount = 22;

    validateInventory(inv, vector<int>(), vector<Slot>(), stone);
    validateChestSingle(chest, vector<int>(), vector<Slot>());

    delete job;
}

TEST_F(BaseChestTest, mode6Test3){
    Inventory2 inv;
    ChestSingle chest;

    for(int i=0;i < 10; i++){
        chest.slots[i] = Slot(1);
    }

    chest.slots[20] = Slot(1);
    chest.slots[20].itemCount = 64;

    for(int i=9;i < 20; i++){
        inv.slots[i] = Slot(1);
    }
    inv.hover = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 6;
    job->button = 0;
    job->slotNum = 21;

    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    Slot stone = Slot(1);
    stone.itemCount = 64;

    Slot stone22 = Slot(1);
    stone22.itemCount = 22;

    validateInventory(inv, vector<int>(), vector<Slot>(), stone);
    validateChestSingle(chest, vector<int>{20}, vector<Slot>{stone22});

    delete job;
}

TEST_F(BaseChestTest, mode6Test4){
    Inventory2 inv;
    ChestSingle chest;

    for(int i=0;i < 10; i++){
        chest.slots[i] = Slot(1);
    }

    chest.slots[20] = Slot(1);
    chest.slots[20].itemCount = 64;

    for(int i=9;i < 20; i++){
        inv.slots[i] = Slot(1);
    }
    inv.hover = Slot(1);

    Slot grassStack = Slot(2);
    grassStack.itemCount = 64;

    chest.slots[21] = grassStack;
    inv.slots[21] = grassStack;

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 6;
    job->button = 0;
    job->slotNum = 21;

    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    Slot stone = Slot(1);
    stone.itemCount = 64;

    Slot stone22 = Slot(1);
    stone22.itemCount = 22;

    validateInventory(inv, vector<int>{21}, vector<Slot>{grassStack}, stone);
    validateChestSingle(chest, vector<int>{20, 21}, vector<Slot>{stone22, grassStack});

    delete job;
}

TEST_F(BaseChestTest, mode6Test5){
    Inventory2 inv;
    ChestSingle chest;

    for(int i=0;i < 10; i++){
        chest.slots[i] = Slot(1);
    }

    chest.slots[4] = Slot(1);
    chest.slots[4].itemCount = 64;

    for(int i=9;i < 20; i++){
        inv.slots[i] = Slot(1);
    }
    inv.hover = Slot(1);

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 6;
    job->button = 0;
    job->slotNum = 21;

    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    Slot stone = Slot(1);
    stone.itemCount = 64;

    Slot stone21 = Slot(1);
    stone21.itemCount = 21;

    validateInventory(inv, vector<int>(), vector<Slot>(), stone);
    validateChestSingle(chest, vector<int>{4}, vector<Slot>{stone21});

    delete job;
}

TEST_F(BaseChestTest, mode6Test6){
    Inventory2 inv;
    ChestSingle chest;

    for(int i=0;i < 10; i++){
        chest.slots[i] = Slot(1);
    }

    for(int i=9;i < 20; i++){
        inv.slots[i] = Slot(1);
    }
    inv.hover = Slot(1);

    inv.slots[13] = Slot(1);
    inv.slots[13].itemCount = 64;

    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 6;
    job->button = 0;
    job->slotNum = 21;

    ClickWindowRequest request;
    request.job = job;
    request.inv = &inv;
    request.altered = &altered;
    request.creative = false;

    chest.clickWindow(request);

    Slot stone = Slot(1);
    stone.itemCount = 64;

    Slot stone21 = Slot(1);
    stone21.itemCount = 21;

    validateInventory(inv, vector<int>{13}, vector<Slot>{stone21}, stone);
    validateChestSingle(chest, vector<int>(), vector<Slot>());

    delete job;
}
