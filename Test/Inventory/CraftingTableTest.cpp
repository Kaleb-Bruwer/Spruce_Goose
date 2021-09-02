#include<gtest/gtest.h>
#include <cmath>

#include "../../src/Inventory/Inventory2.h"
#include "../../src/Inventory/BlockData/CraftingTable.h"

class CraftingTableTest : public ::testing::Test{
protected:
    AlteredSlots altered;
    Inventory2 inventory2;
    CraftingTable craftingTable;

    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

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
        craftingTable.clickWindow(job, &inventory2, altered, false);

        ASSERT_TRUE(inventory2.hover == s) << "Incorrect item";
        if(slot < 10)
            ASSERT_TRUE(craftingTable.slots[slot] == h) << "Incorrect item";
        else
            ASSERT_TRUE(inventory2.slots[slot-1] == h) << "Incorrect item";

        delete job;

    }

    void testMode0Btn1(Slot s, Slot h, int slot = 9){
        ClickWindowJob* job = initJob(0, slot);
        job->mode = 0;
        job->button = 1;

        inventory2.slots[slot] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);

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

        ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
        ASSERT_TRUE(inventory2.slots[slot] == sExpect) << "slot: " << inventory2.slots[slot] << ", expected: " << sExpect;

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

    craftingTable.clickWindow(job, &inventory2, altered, false);

    ASSERT_TRUE(inventory2.hover == h) << "Incorrect item";
    ASSERT_TRUE(craftingTable.slots[slot] == s) << "Incorrect item";

    delete job;
}

TEST_F(CraftingTableTest, testMode0Btn0T8){
    Slot s = getStone();
    Slot h = Slot();
    int slot = 0;

    ClickWindowJob* job = initJob(0, slot);
    job->button = 0;

    craftingTable.slots[slot] = s;
    inventory2.hover = h;

    craftingTable.clickWindow(job, &inventory2, altered, false);

    ASSERT_TRUE(inventory2.hover == s) << "hover: " << inventory2.hover;
    ASSERT_TRUE(craftingTable.slots[slot] == h) << "slot: " << inventory2.slots[slot];

    delete job;
}


// items both sides
TEST_F(CraftingTableTest, testMode0Btn0T9){
    testMode0Btn0(getStone(), getDirt(), 1);
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
    testMode0Btn1(getDirt(64), getStone(12), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T13){
    testMode0Btn1(getDirt(64), getStone(), 1);
}

TEST_F(CraftingTableTest, testMode0Btn1T14){
    testMode0Btn1(getDirt(30), getStone(64), 1);
}


// shift clicks

// crafting frame into inventory
TEST_F(CraftingTableTest, testMode1T1){
    // Set up inventory
    craftingTable.slots[1] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 1);

    // execute
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot1: " << craftingTable.slots[1] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;

    delete job;
}

// hotbar to empty inventory, but through crafting table
TEST_F(CraftingTableTest, testMode1T2){
    // Set up inventory
    inventory2.slots[36] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 37);

    // execute
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;

    delete job;
}

// inventory to hotbar via crafting table
TEST_F(CraftingTableTest, testMode1T3){
    // Set up inventory
    inventory2.slots[9] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 10);

    // execute
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[36] == sExpect2) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect2;

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
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone(2);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot1: " << craftingTable.slots[1] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[21] == sExpect2) << "slot21: " << inventory2.slots[21] << ", expected: " << sExpect2;

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
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();
    Slot sExpect2 = getStone(64);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot1: " << craftingTable.slots[1] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[21] == sExpect2) << "slot21: " << inventory2.slots[21] << ", expected: " << sExpect2;

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
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();
    Slot sExpect2 = getStone(64);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[21] == sExpect2) << "slot21: " << inventory2.slots[21] << ", expected: " << sExpect2;

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
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot1: " << craftingTable.slots[1] << ", expected: " << sExpect1;

    for(int i=9; i<45; i++){
        ASSERT_TRUE(inventory2.slots[i] == sExpect2) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect2;
    }

    delete job;
}


// CraftingTable to inventory, must split between two near-full stacks
TEST_F(CraftingTableTest, testMode1T10){
    // Set up inventory
    craftingTable.slots[1] = getStone(4);
    inventory2.slots[14] = getStone(62);
    inventory2.slots[16] = getStone(62);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect3 = getStone(64);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot1: " << craftingTable.slots[1] << ", expected: " << sExpect1;

    for(int i=0; i<45; i++)
        if(i != 14 && i != 16)
            ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    ASSERT_TRUE(inventory2.slots[14] == sExpect3) << "slot14: " << inventory2.slots[14] << ", expected: " << sExpect3;
    ASSERT_TRUE(inventory2.slots[16] == sExpect3) << "slot16: " << inventory2.slots[16] << ", expected: " << sExpect3;

    delete job;
}

// MODE 2: numeric keys

// move item away
TEST_F(CraftingTableTest, testMode2T1){
    //Set up inventory
    craftingTable.slots[1] = getStone();

    ClickWindowJob* job = initJob(2, 1);
    job->button = 4;

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot1: " << craftingTable.slots[1] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// move item in
TEST_F(CraftingTableTest, testMode2T2){
    //Set up inventory
    inventory2.slots[37] = getStone();

    ClickWindowJob* job = initJob(2, 1);
    job->button = 1;

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot36: " << craftingTable.slots[1] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[37] == sExpect2) << "slot40: " << inventory2.slots[37] << ", expected: " << sExpect2;

    delete job;
}

// swap hotbar items via crafting table
TEST_F(CraftingTableTest, testMode2T3){
    //Set up inventory
    inventory2.slots[37] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 41);
    job->button = 1;

    Slot hExpect = Slot();
    Slot sExpect1 = getDirt();
    Slot sExpect2 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[37] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// swap from main inventory via crafting table
TEST_F(CraftingTableTest, testMode2T4){
    //Set up inventory
    inventory2.slots[10] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 11);
    job->button = 4;

    Slot hExpect = Slot();
    Slot sExpect1 = getDirt();
    Slot sExpect2 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[10] == sExpect1) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// middle click in survival
TEST_F(CraftingTableTest, testMode3T1){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3, 10);
    job->button = 2;

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;

    delete job;
}

// middle click in creative
TEST_F(CraftingTableTest, testMode3T2){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3, 10);
    job->button = 2;

    Slot hExpect = getStone(64);
    Slot sExpect1 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;

    delete job;
}

// middle click in survival in crafting frame
TEST_F(CraftingTableTest, testMode3T3){
    //Set up inventory
    craftingTable.slots[1] = getStone();

    ClickWindowJob* job = initJob(3, 1);
    job->button = 2;

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot9: " << craftingTable.slots[1] << ", expected: " << sExpect1;

    delete job;
}

// middle click in creative
TEST_F(CraftingTableTest, testMode3T4){
    //Set up inventory
    inventory2.slots[1] = getStone();

    ClickWindowJob* job = initJob(3, 1);
    job->button = 2;

    Slot hExpect = getStone(64);
    Slot sExpect1 = getStone();

    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[1] == sExpect1) << "slot9: " << craftingTable.slots[1] << ", expected: " << sExpect1;

    delete job;
}



// simple mouse drag
TEST_F(CraftingTableTest, testMode5T1){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5, 10);
    job->button = 0;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to left drag
    job->slotNum = 10;
    job->button = 1;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    job->slotNum = 11;
    job->button = 1;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // end drag
    job->slotNum = 11;
    job->button = 2;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone(32);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[10] == sExpect1) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect1;

    delete job;
}

// mouse drag, crosses from crafting table into inventory
TEST_F(CraftingTableTest, testMode5T2){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5,10);
    job->button = 0;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to left drag
    for(int i=8; i<11; i++){
        job->slotNum = i;
        job->button = 1;
        craftingTable.clickWindow(job, &inventory2, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone();
    Slot sExpect1 = getStone(21);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[9] == sExpect1) << "slot: " << craftingTable.slots[9] << ", expected: " << sExpect1;
    for(int i=9; i<11; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

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
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to left drag
    for(int i=8; i<12; i++){
        job->slotNum = i;
        job->button = 1;
        craftingTable.clickWindow(job, &inventory2, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone();
    Slot sExpect1 = getStone(21);
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[9] == sExpect1) << "slot: " << craftingTable.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot: " << inventory2.slots[9] << ", expected: " << sExpect2;
    for(int i=10; i<12; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    delete job;
}

// mouse drag with too few items
TEST_F(CraftingTableTest, testMode5T4){
    //Set up inventory
    inventory2.hover = getStone(4);

    // Start left drag
    ClickWindowJob* job = initJob(5,10);
    job->button = 0;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to left drag
    for(int i=8; i<14; i++){
        job->slotNum = i;
        job->button = 1;
        craftingTable.clickWindow(job, &inventory2, altered, false);
    }

    // end drag
    job->slotNum = 13;
    job->button = 2;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone();
    Slot sExpect1 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[9] == sExpect1) << "slot: " << craftingTable.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(craftingTable.slots[10] == sExpect1) << "slot: " << craftingTable.slots[10] << ", expected: " << sExpect1;

    for(int i=9; i<11; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    for(int i=11; i<45; i++)
        ASSERT_TRUE(inventory2.slots[i] == Slot()) << "slot: " << inventory2.slots[i] << ", expected: " << Slot();

    delete job;
}

// simple right mouse drag
TEST_F(CraftingTableTest, testMode5T5){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start right drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to right drag
    job->slotNum = 9;
    job->button = 5;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    job->slotNum = 10;
    job->button = 5;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // end drag
    job->slotNum = 10;
    job->button = 6;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone(62);
    Slot sExpect1 = getStone(1);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(craftingTable.slots[9] == sExpect1) << "slot9: " << craftingTable.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;

    delete job;
}

// simple mouse drag (5 slots)
TEST_F(CraftingTableTest, testMode5T6){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start right drag
    ClickWindowJob* job = initJob(5,8);
    job->button = 4;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to right drag
    for(int i=8; i<13; i++){
        job->slotNum = i;
        job->button = 5;
        craftingTable.clickWindow(job, &inventory2, altered, false);
    }

    // end drag
    job->slotNum = 12;
    job->button = 6;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone(59);
    Slot sExpect1 = getStone(1);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=8; i<10; i++)
        ASSERT_TRUE(craftingTable.slots[i] == sExpect1) << "slot: " << craftingTable.slots[i] << ", expected: " << sExpect1;

    for(int i=9; i<14; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    delete job;
}

// right mouse drag with too few items
TEST_F(CraftingTableTest, testMode5T7){
    //Set up inventory
    inventory2.hover = getStone(4);

    // Start left drag
    ClickWindowJob* job = initJob(5,8);
    job->button = 4;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to left drag
    for(int i=8; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        craftingTable.clickWindow(job, &inventory2, altered, false);
    }

    // end drag
    job->slotNum = 13;
    job->button = 6;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone(1);
    Slot sExpect2 = Slot();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=8; i<10; i++)
        ASSERT_TRUE(craftingTable.slots[i] == sExpect1) << "slot: " << craftingTable.slots[i] << ", expected: " << sExpect1;
    for(int i=9; i<11; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    for(int i=11; i<45; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect2) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect2;

    delete job;
}

// simple mouse drag (5 slots)
TEST_F(CraftingTableTest, testMode5T8){
    //Set up inventory
    inventory2.hover = getStone(64);
    craftingTable.slots[9] = getDirt();

    // Start left drag
    ClickWindowJob* job = initJob(5, 8);
    job->button = 4;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    // Add items to left drag
    for(int i=8; i<14; i++){
        job->slotNum = i;
        job->button = 5;
    craftingTable.clickWindow(job, &inventory2, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone(59);
    Slot sExpect1 = getStone();
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;

    ASSERT_TRUE(craftingTable.slots[8] == sExpect1) << "slot: " << craftingTable.slots[8] << ", expected: " << sExpect1;
    ASSERT_TRUE(craftingTable.slots[9] == sExpect2) << "slot: " << craftingTable.slots[9] << ", expected: " << sExpect2;

    for(int i=9; i<13; i++){
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot10: " << inventory2.slots[i] << ", expected: " << sExpect1;
    }

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
    craftingTable.clickWindow(job, &inventory2, altered, false);

    job->mode = 6;
    craftingTable.clickWindow(job, &inventory2, altered, false);

    Slot hExpect = getStone(10);
    Slot sExpect1 = Slot();

    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=0; i<45; i++){
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;
    }

    delete job;
}
