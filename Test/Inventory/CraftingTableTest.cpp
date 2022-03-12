#include<gtest/gtest.h>
#include <cmath>

#include "CommonHelpers.cpp"

#include "../../src/Inventory/Inventory2.h"
#include "../../src/Inventory/BlockData/CraftingTable.h"

class CraftingTableTest : public ::testing::Test{
protected:
    AlteredSlots altered;
    Inventory2 inventory2;
    CraftingTable craftingTable;

    void callClickWindow(ClickWindowJob* job, bool creative = false){
        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory2;
        request.altered = &altered;
        request.creative = creative;

        craftingTable.clickWindow(request);
    }


    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

    void validateInventory(vector<int> pos, vector<Slot> items, Slot hover){
        ::validateInventory(inventory2, pos, items, hover);
    }

    void validateCraftingTable(vector<int> pos, vector<Slot> items){
        // Excludes product (slot 0), unless specificly added
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
        if(pos.size() > 0 && pos[0] == 0){
            ASSERT_TRUE(craftingTable.slots[0] == items[0])
                << "[0]: " << craftingTable.slots[0] << ", expected: " << items[0];
            specialPos++;
        }

        for(int i=1; i<10; i++){
            if(specialPos < pos.size() && pos[specialPos] == i){
                ASSERT_TRUE(craftingTable.slots[i] == items[specialPos])
                    << "[" << i << "]: " << craftingTable.slots[i] << ", expected: " << items[specialPos];
                specialPos++;
            }
            else{
                ASSERT_TRUE(craftingTable.slots[i].isEmpty())
                    << "[" << i << "]: " << craftingTable.slots[i] << ", expected: empty";
            }
        }
    }

    ClickWindowJob* initJob(int mode = 0, int slotNum = 9){
        ClickWindowJob* result = new ClickWindowJob();

        result->windowID = 0;
        result->mode = mode;
        result->slotNum = slotNum;

        return result;
    };

    Slot getStone(int count = 1){
        Slot result;
        result.itemID = 1;
        result.itemCount = count;
        return result;
    };

    Slot getDirt(int count = 1){
        Slot result;
        result.itemID = 3;
        result.itemCount = count;
        return result;
    };

    void testMode0Btn0(Slot s, Slot h, int slot = 9){
        ClickWindowJob* job = initJob(0, slot);
        job->mode = 0;
        job->button = 0;

        if(slot < 10)
            craftingTable.slots[slot] = s;
        else
            inventory2.slots[slot-1] = s;

        inventory2.hover = h;

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory2;
        request.altered = &altered;
        request.creative = false;

        craftingTable.clickWindow(request);

        Slot sExpect = h;
        Slot hExpect = s;
        if(s.typeMatch(h)){
            sExpect.itemCount = s.itemCount + h.itemCount;
            int maxStack = sExpect.maxStackSize();
            int leftover = sExpect.itemCount - maxStack;
            if(leftover <= 0){
                hExpect.makeEmpty();
            }
            else{
                hExpect.itemCount = leftover;
                sExpect.itemCount = maxStack;
            }
        }

        if(slot < 10){
            validateCraftingTable(vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(vector<int>{slot-1}, vector<Slot>{hExpect}, sExpect);
            validateCraftingTable(vector<int>(), vector<Slot>());
        }
        delete job;
    }

    void testMode0Btn1(Slot s, Slot h, int slot = 9){
        ClickWindowJob* job = initJob(0, slot);
        job->mode = 0;
        job->button = 1;

        if(slot < 10)
            craftingTable.slots[slot] = s;
        else
            inventory2.slots[slot-1] = s;

        inventory2.hover = h;

        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory2;
        request.altered = &altered;
        request.creative = false;

        craftingTable.clickWindow(request);

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

        if(slot < 10){
            validateInventory(vector<int>(), vector<Slot>(), hExpect);
            validateCraftingTable(vector<int>{slot}, vector<Slot>{sExpect});
        }
        else{
            validateInventory(vector<int>{slot - 1}, vector<Slot>{sExpect}, hExpect);
            validateCraftingTable(vector<int>(), vector<Slot>());
        }

        // ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
        // ASSERT_TRUE(inventory2.slots[slot] == sExpect) << "slot: " << inventory2.slots[slot] << ", expected: " << sExpect;

        delete job;
    }

};

// Single item
TEST_F(CraftingTableTest, testMode0Btn0T1){
    testMode0Btn0(getStone(), Slot(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T2){
    testMode0Btn0(Slot(), getStone(), 1);
}

// multiple items
TEST_F(CraftingTableTest, testMode0Btn0T3){
    testMode0Btn0(getStone(5), Slot(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T4){
    testMode0Btn0(Slot(), getStone(5), 1);
}

// Full stack
TEST_F(CraftingTableTest, testMode0Btn0T5){
    testMode0Btn0(getStone(64), Slot(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T6){
    testMode0Btn0(Slot(), getStone(64), 1);
}

// can't place in crafting result
TEST_F(CraftingTableTest, testMode0Btn0T7){
    Slot s = Slot();
    Slot h = getStone();
    int slot = 0;

    ClickWindowJob* job = initJob(0, slot);
    job->button = 0;

    craftingTable.slots[slot] = s;
    inventory2.hover = h;

    // callClickWindow(job);
    callClickWindow(job);

    validateInventory(vector<int>(), vector<Slot>(), h);
    validateCraftingTable(vector<int>{slot}, vector<Slot>{s});
    delete job;
}

// items both sides
TEST_F(CraftingTableTest, testMode0Btn0T9){
    testMode0Btn0(getStone(), getDirt(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T10){
    testMode0Btn0(getStone(5), getStone(3), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T11){
    testMode0Btn0(getStone(), getStone(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T12){
    testMode0Btn0(getStone(64), getStone(3), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T13){
    testMode0Btn0(getStone(3), getStone(64), 1);
}

TEST_F(CraftingTableTest, testMode0Btn0T14){
    testMode0Btn0(getStone(62), getStone(3), 1);
}

// Single item
TEST_F(CraftingTableTest, testMode0Btn1T1){
    testMode0Btn1(getStone(), Slot(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T2){
    testMode0Btn1(Slot(), getStone(), 1);
}

// multiple items
TEST_F(CraftingTableTest, testMode0Btn1T3){
    testMode0Btn1(getStone(12), Slot(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T4){
    testMode0Btn1(getStone(11), Slot(), 1);
}

// Place single item
TEST_F(CraftingTableTest, testMode0Btn1T5){
    testMode0Btn1(Slot(), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T6){
    testMode0Btn1(Slot(), getStone(11), 1);
}

// mixed item types
TEST_F(CraftingTableTest, testMode0Btn1T7){
    testMode0Btn1(getDirt(), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T8){
    testMode0Btn1(getDirt(15), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T9){
    testMode0Btn1(getDirt(64), getStone(12), 1);
}

// same item both sides
TEST_F(CraftingTableTest, testMode0Btn1T10){
    testMode0Btn1(getStone(), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T11){
    testMode0Btn1(getStone(15), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T12){
    testMode0Btn1(getStone(64), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T13){
    testMode0Btn1(getDirt(64), getStone(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T14){
    testMode0Btn1(getDirt(30), getStone(64), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T15){
    testMode0Btn1(getStone(63), getStone(4), 1);
}

// shift clicks

// crafting frame into inventory
TEST_F(CraftingTableTest, testMode1T1){
    // Set up inventory
    craftingTable.slots[1] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    // callClickWindow(job);
    callClickWindow(job);

    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// hotbar to empty inventory, but through crafting table
TEST_F(CraftingTableTest, testMode1T2){
    // Set up inventory
    inventory2.slots[36] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 37);

    // execute
    callClickWindow(job);

    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());

    delete job;
}

// inventory to hotbar via crafting table
TEST_F(CraftingTableTest, testMode1T3){
    // Set up inventory
    inventory2.slots[9] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 10);

    // execute
    callClickWindow(job);

    validateInventory(vector<int>{36}, vector<Slot>{getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());

    delete job;
}

// crafting table to inventory that has a partial stack
TEST_F(CraftingTableTest, testMode1T4){
    // Set up inventory
    craftingTable.slots[1] = getStone();
    inventory2.slots[21] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    callClickWindow(job);

    validateInventory(vector<int>{21}, vector<Slot>{getStone(2)}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());

    delete job;
}

// Crafting table to inventory that has a full stack
TEST_F(CraftingTableTest, testMode1T5){
    // Set up inventory
    craftingTable.slots[1] = getStone();
    inventory2.slots[21] = getStone(64);

    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    callClickWindow(job);

    validateInventory(vector<int>{9, 21}, vector<Slot>{getStone(), getStone(64)}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// Crafting table to inventory that has an almost-full stack
TEST_F(CraftingTableTest, testMode1T7){
    // Set up inventory
    craftingTable.slots[1] = getStone(5);
    inventory2.slots[21] = getStone(60);


    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    callClickWindow(job);

    validateInventory(vector<int>{9, 21}, vector<Slot>{getStone(), getStone(64)}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// From crafting table to full inventory
TEST_F(CraftingTableTest, testMode1T8){
    // Set up inventory
    craftingTable.slots[1] = getStone();

    for(int i=9; i < 45; i++){
        inventory2.slots[i] = getDirt();
    }

    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    callClickWindow(job);

    vector<int> pos;
    vector<Slot> item;

    for(int i=9; i<45; i++){
        pos.push_back(i);
        item.push_back(getDirt());
    }

    validateInventory(pos, item, Slot());
    validateCraftingTable(vector<int>{1}, vector<Slot>{getStone()});
    delete job;
}


// CraftingTable to inventory, must split between two near-full stacks
TEST_F(CraftingTableTest, testMode1T10){
    // Set up inventory
    craftingTable.slots[1] = getStone(4);
    inventory2.slots[14] = getStone(62);
    inventory2.slots[16] = getStone(62);

    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    callClickWindow(job);

    validateInventory(vector<int>{14, 16}, vector<Slot>{getStone(64), getStone(64)}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());

    delete job;
}

// MODE 2: numeric keys

// move item away
TEST_F(CraftingTableTest, testMode2T1){
    //Set up inventory
    craftingTable.slots[1] = getStone();

    ClickWindowJob* job = initJob(2, 1);
    job->button = 4;

    callClickWindow(job);

    validateInventory(vector<int>{40}, vector<Slot>{getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// move item in
TEST_F(CraftingTableTest, testMode2T2){
    //Set up inventory
    inventory2.slots[37] = getStone();

    ClickWindowJob* job = initJob(2, 1);
    job->button = 1;

    callClickWindow(job);

    validateInventory(vector<int>(), vector<Slot>(), Slot());
    validateCraftingTable(vector<int>{1}, vector<Slot>{getStone()});
    delete job;
}

// swap hotbar items via crafting table
TEST_F(CraftingTableTest, testMode2T3){
    //Set up inventory
    inventory2.slots[37] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 41);
    job->button = 1;

    callClickWindow(job);

    validateInventory(vector<int>{37, 40}, vector<Slot>{getDirt(), getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// swap from main inventory via crafting table
TEST_F(CraftingTableTest, testMode2T4){
    //Set up inventory
    inventory2.slots[10] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 11);
    job->button = 4;

    callClickWindow(job);

    validateInventory(vector<int>{10, 40}, vector<Slot>{getDirt(), getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// middle click in survival
TEST_F(CraftingTableTest, testMode3T1){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3, 10);
    job->button = 2;

    callClickWindow(job);

    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// middle click in creative
TEST_F(CraftingTableTest, testMode3T2){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3, 10);
    job->button = 2;

    callClickWindow(job, true);

    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, getStone(64));
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// middle click in survival in crafting frame
TEST_F(CraftingTableTest, testMode3T3){
    //Set up inventory
    craftingTable.slots[1] = getStone();

    ClickWindowJob* job = initJob(3, 1);
    job->button = 2;

    callClickWindow(job);

    validateInventory(vector<int>(), vector<Slot>(), Slot());
    validateCraftingTable(vector<int>{1}, vector<Slot>{getStone()});

    delete job;
}

// middle click in creative
TEST_F(CraftingTableTest, testMode3T4){
    //Set up inventory
    craftingTable.slots[1] = getStone();

    ClickWindowJob* job = initJob(3, 1);
    job->button = 2;

    callClickWindow(job, true);

    validateInventory(vector<int>(), vector<Slot>(), getStone(64));
    validateCraftingTable(vector<int>{1}, vector<Slot>{getStone()});

    delete job;
}



// simple mouse drag
TEST_F(CraftingTableTest, testMode5T1){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5, 10);
    job->button = 0;
    callClickWindow(job);

    // Add items to left drag
    job->slotNum = 10;
    job->button = 1;
    callClickWindow(job);

    job->slotNum = 11;
    job->button = 1;
    callClickWindow(job);

    // end drag
    job->slotNum = 11;
    job->button = 2;
    callClickWindow(job);


    validateInventory(vector<int>{9, 10}, vector<Slot>{getStone(32), getStone(32)}, Slot());
    validateCraftingTable(vector<int>(), vector<Slot>());
    delete job;
}

// mouse drag, crosses from crafting table into inventory
TEST_F(CraftingTableTest, testMode5T2){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5,10);
    job->button = 0;
    callClickWindow(job);

    // Add items to left drag
    for(int i=8; i<11; i++){
        job->slotNum = i;
        job->button = 1;
        callClickWindow(job);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    callClickWindow(job);

    validateInventory(vector<int>{9}, vector<Slot>{getStone(21)}, getStone());
    validateCraftingTable(vector<int>{8,9}, vector<Slot>{getStone(21), getStone(21)});
    delete job;
}

// mouse drag, crosses other item
TEST_F(CraftingTableTest, testMode5T3){
    //Set up inventory
    inventory2.hover = getStone(64);
    inventory2.slots[9] = getDirt();

    // Start left drag
    ClickWindowJob* job = initJob(5,10);
    job->button = 0;
    callClickWindow(job);

    // Add items to left drag
    for(int i=8; i<12; i++){
        job->slotNum = i;
        job->button = 1;
        callClickWindow(job);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    callClickWindow(job);

    validateInventory(vector<int>{9, 10}, vector<Slot>{getDirt(), getStone(21)}, getStone());
    validateCraftingTable(vector<int>{8, 9}, vector<Slot>(2, getStone(21)));
    delete job;
}

// mouse drag with too few items
TEST_F(CraftingTableTest, testMode5T4){
    //Set up inventory
    inventory2.hover = getStone(4);

    // Start left drag
    ClickWindowJob* job = initJob(5,10);
    job->button = 0;
    callClickWindow(job);

    // Add items to left drag
    for(int i=8; i<14; i++){
        job->slotNum = i;
        job->button = 1;
        callClickWindow(job);
    }

    // end drag
    job->slotNum = 13;
    job->button = 2;
    callClickWindow(job);

    validateInventory(vector<int>{9, 10}, vector<Slot>{getStone(), getStone()}, Slot());
    validateCraftingTable(vector<int>{8, 9}, vector<Slot>{getStone(), getStone()});
    delete job;
}

// simple right mouse drag
TEST_F(CraftingTableTest, testMode5T5){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start right drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    callClickWindow(job);

    // Add items to right drag
    job->slotNum = 9;
    job->button = 5;
    callClickWindow(job);

    job->slotNum = 10;
    job->button = 5;
    callClickWindow(job);

    // end drag
    job->slotNum = 10;
    job->button = 6;
    callClickWindow(job);

    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, getStone(62));
    validateCraftingTable(vector<int>{9}, vector<Slot>{getStone()});
    delete job;
}

// simple mouse drag (5 slots)
TEST_F(CraftingTableTest, testMode5T6){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start right drag
    ClickWindowJob* job = initJob(5,8);
    job->button = 4;
    callClickWindow(job);

    // Add items to right drag
    for(int i=8; i<13; i++){
        job->slotNum = i;
        job->button = 5;
        callClickWindow(job);
    }

    // end drag
    job->slotNum = 12;
    job->button = 6;
    callClickWindow(job);

    validateInventory(vector<int>{9, 10, 11}, vector<Slot>(3, getStone()), getStone(59));
    validateCraftingTable(vector<int>{8, 9}, vector<Slot>{getStone(), getStone()});
    delete job;
}

// right mouse drag with too few items
TEST_F(CraftingTableTest, testMode5T7){
    //Set up inventory
    inventory2.hover = getStone(4);

    // Start left drag
    ClickWindowJob* job = initJob(5,8);
    job->button = 4;
    callClickWindow(job);

    // Add items to left drag
    for(int i=8; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        callClickWindow(job);
    }

    // end drag
    job->slotNum = 13;
    job->button = 6;
    callClickWindow(job);

    validateInventory(vector<int>{9, 10}, vector<Slot>{getStone(), getStone()}, Slot());
    validateCraftingTable(vector<int>{8, 9}, vector<Slot>{getStone(), getStone()});
    delete job;
}

// simple mouse drag (5 slots)
TEST_F(CraftingTableTest, testMode5T8){
    //Set up inventory
    inventory2.hover = getStone(64);
    craftingTable.slots[9] = getDirt();

    // Start right drag
    ClickWindowJob* job = initJob(5, 8);
    job->button = 4;
    callClickWindow(job);

    // Add items to left drag
    for(int i=8; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        callClickWindow(job);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    callClickWindow(job);

    validateInventory(vector<int>{9, 10, 11, 12}, vector<Slot>(4, getStone()), getStone(59));
    validateCraftingTable(vector<int>{8, 9}, vector<Slot>{getStone(), getDirt()});
    delete job;
}

// Normal double click (incl. preceding single-click)
TEST_F(CraftingTableTest, testMode6T1){
    //Set up inventory
    inventory2.hover = Slot();
    for(int i=9; i<19; i++)
        inventory2.slots[i] = getStone();

    // left click
    ClickWindowJob* job = initJob(0, 10);
    job->button = 0;
    callClickWindow(job);

    job->mode = 6;
    callClickWindow(job);

    validateInventory(vector<int>(), vector<Slot>(), getStone(10));
    validateCraftingTable(vector<int>(), vector<Slot>());

    delete job;
}

TEST_F(CraftingTableTest, testCraft1){
    // Craft sticks & pick them up (items start in hover)

    // craftingTable.slots[1] = Slot(5);
    // craftingTable.slots[4] = Slot(5);

    inventory2.hover = Slot(5);
    inventory2.hover.itemCount = 10;

    ClickWindowJob* job = initJob(0, 1);
    job->button = 1;
    callClickWindow(job);

    Slot hExpect = Slot(5);
    hExpect.itemCount = 9;

    // Intermediate check
    validateInventory(vector<int>(), vector<Slot>(), hExpect);
    validateCraftingTable(vector<int>{1}, vector<Slot>{Slot(5)});

    job->slotNum = 4;
    callClickWindow(job);

    hExpect.itemCount--;
    Slot sticks = Slot(280);
    sticks.itemCount = 4;

    // Check crafting works
    validateInventory(vector<int>(), vector<Slot>(), hExpect);
    validateCraftingTable(vector<int>{0, 1, 4}, vector<Slot>{sticks, Slot(5), Slot(5)});

    job->slotNum = 10;
    job->button = 0;
    callClickWindow(job);

    job->slotNum = 0;
    callClickWindow(job);

    // Pick up result
    validateInventory(vector<int>{9}, vector<Slot>{hExpect}, sticks);
    validateCraftingTable(vector<int>(), vector<Slot>());
}

TEST_F(CraftingTableTest, testCraft2){
    // Craft pickaxe (items start in hover & inventory)

    // craftingTable.slots[1] = Slot(5);
    // craftingTable.slots[4] = Slot(5);
    Slot cobble = Slot(4);
    cobble.itemCount = 3;

    inventory2.hover = Slot(280);
    inventory2.hover.itemCount = 2;
    inventory2.slots[9] = cobble;

    ClickWindowJob* job = initJob(0, 5);
    job->button = 1;
    callClickWindow(job);
    job->slotNum = 8;
    callClickWindow(job);

    // Intermediate
    validateInventory(vector<int>{9}, vector<Slot>{cobble}, Slot());
    validateCraftingTable(vector<int>{5, 8}, vector<Slot>{Slot(280), Slot(280)});

    job->slotNum = 10;
    job->button = 0;
    callClickWindow(job);

    job->button = 1;
    job->slotNum = 1;
    callClickWindow(job);
    job->slotNum = 2;
    callClickWindow(job);
    job->slotNum = 3;
    callClickWindow(job);

    Slot pickaxe = Slot(274);

    // Check Crafting
    validateInventory(vector<int>(), vector<Slot>(), Slot());
    validateCraftingTable(vector<int>{0, 1, 2, 3, 5, 8},
        vector<Slot>{pickaxe, Slot(4), Slot(4), Slot(4), Slot(280), Slot(280)});

    job->button  = 0;
    job->slotNum = 0;
    callClickWindow(job);

    validateInventory(vector<int>(), vector<Slot>(), pickaxe);
    validateCraftingTable(vector<int>(), vector<Slot>());
}

TEST_F(CraftingTableTest, testCraft3){
    // Craft sticks with excess input

    inventory2.hover = Slot(5);
    inventory2.hover.itemCount = 10;

    ClickWindowJob* job = initJob(0, 1);
    job->button = 1;
    callClickWindow(job);
    callClickWindow(job);

    Slot hExpect = Slot(5);
    hExpect.itemCount = 8;

    Slot sExpect = Slot(5);
    sExpect.itemCount = 2;
    // Intermediate check
    validateInventory(vector<int>(), vector<Slot>(), hExpect);
    validateCraftingTable(vector<int>{1}, vector<Slot>{sExpect});

    job->slotNum = 4;
    callClickWindow(job);

    hExpect.itemCount--;
    Slot sticks = Slot(280);
    sticks.itemCount = 4;

    // Check crafting works
    validateInventory(vector<int>(), vector<Slot>(), hExpect);
    validateCraftingTable(vector<int>{0, 1, 4}, vector<Slot>{sticks, sExpect, Slot(5)});

    job->slotNum = 10;
    job->button = 0;
    callClickWindow(job);

    job->slotNum = 0;
    callClickWindow(job);

    // Pick up result
    validateInventory(vector<int>{9}, vector<Slot>{hExpect}, sticks);
    validateCraftingTable(vector<int>{1}, vector<Slot>{Slot(5)});

}

TEST_F(CraftingTableTest, testCraft4){
    // Craft sticks with excess input

    inventory2.hover = Slot(5);
    inventory2.hover.itemCount = 10;

    ClickWindowJob* job = initJob(0, 1);
    job->button = 1;
    callClickWindow(job);
    callClickWindow(job);

    Slot hExpect = Slot(5);
    hExpect.itemCount = 8;

    Slot sExpect = Slot(5);
    sExpect.itemCount = 2;
    // Intermediate check
    validateInventory(vector<int>(), vector<Slot>(), hExpect);
    validateCraftingTable(vector<int>{1}, vector<Slot>{sExpect});

    job->slotNum = 4;
    callClickWindow(job);
    callClickWindow(job);

    hExpect.itemCount -= 2;
    Slot sticks = Slot(280);
    sticks.itemCount = 4;

    // Check crafting works
    validateInventory(vector<int>(), vector<Slot>(), hExpect);
    validateCraftingTable(vector<int>{0, 1, 4}, vector<Slot>{sticks, sExpect, sExpect});

    job->slotNum = 10;
    job->button = 0;
    callClickWindow(job);

    job->slotNum = 0;
    callClickWindow(job);

    // Pick up result
    validateInventory(vector<int>{9}, vector<Slot>{hExpect}, sticks);
    validateCraftingTable(vector<int>{1, 4}, vector<Slot>{Slot(5), Slot(5)});

    callClickWindow(job);

    sticks.itemCount = 8;
    validateInventory(vector<int>{9}, vector<Slot>{hExpect}, sticks);
    validateCraftingTable(vector<int>(), vector<Slot>());

}

TEST_F(CraftingTableTest, testCraft5){
    // Craft wooden pickaxe (items start in hover & inventory)

    // craftingTable.slots[1] = Slot(5);
    // craftingTable.slots[4] = Slot(5);
    Slot cobble = Slot(5);
    cobble.itemDamage = 1;
    cobble.itemCount = 3;

    inventory2.hover = Slot(280);
    inventory2.hover.itemCount = 2;
    inventory2.slots[9] = cobble;

    ClickWindowJob* job = initJob(0, 5);
    job->button = 1;
    callClickWindow(job);
    job->slotNum = 8;
    callClickWindow(job);

    // Intermediate
    validateInventory(vector<int>{9}, vector<Slot>{cobble}, Slot());
    validateCraftingTable(vector<int>{5, 8}, vector<Slot>{Slot(280), Slot(280)});

    job->slotNum = 10;
    job->button = 0;
    callClickWindow(job);


    job->mode = 5;
    job->button = 4;
    job->slotNum = -999;
    callClickWindow(job);

    job->button = 5;
    job->slotNum = 1;
    callClickWindow(job);
    job->slotNum = 2;
    callClickWindow(job);
    job->slotNum = 3;
    callClickWindow(job);

    job->button = 6;
    job->slotNum = -999;
    callClickWindow(job);

    Slot pickaxe = Slot(270);

    // Check Crafting

    cobble.itemCount = 1;

    validateInventory(vector<int>(), vector<Slot>(), Slot());
    validateCraftingTable(vector<int>{0, 1, 2, 3, 5, 8},
        vector<Slot>{pickaxe, cobble, cobble, cobble, Slot(280), Slot(280)});

    job->mode = 0;
    job->button  = 0;
    job->slotNum = 0;
    callClickWindow(job);

    validateInventory(vector<int>(), vector<Slot>(), pickaxe);
    validateCraftingTable(vector<int>(), vector<Slot>());
}
