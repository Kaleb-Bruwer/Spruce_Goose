#include<gtest/gtest.h>
#include <cmath>

#include "../../src/Inventory/Inventory2.h"

class Inventory2Test : public ::testing::Test{
protected:
    AlteredSlots altered;
    Inventory2 inventory2;

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

        inventory2.slots[slot] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);

        ASSERT_TRUE(inventory2.hover == s) << "Incorrect item";
        ASSERT_TRUE(inventory2.slots[slot] == h) << "Incorrect item";

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
TEST_F(Inventory2Test, testMode0Btn0T1){
    testMode0Btn0(getStone(), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T2){
    testMode0Btn0(Slot(), getStone());
}

// multiple items
TEST_F(Inventory2Test, testMode0Btn0T3){
    testMode0Btn0(getStone(5), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T4){
    testMode0Btn0(Slot(), getStone(5));
}

// Full stack
TEST_F(Inventory2Test, testMode0Btn0T5){
    testMode0Btn0(getStone(64), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T6){
    testMode0Btn0(Slot(), getStone(64));
}

// can't place in crafting result
TEST_F(Inventory2Test, testMode0Btn0T7){
    Slot s = Slot();
    Slot h = getStone();
    int slot = 0;

    ClickWindowJob* job = initJob(0, slot);
    job->button = 0;

    inventory2.slots[slot] = s;
    inventory2.hover = h;

    inventory2.clickWindow(job, 0, altered, false);

    ASSERT_TRUE(inventory2.hover == h) << "Incorrect item";
    ASSERT_TRUE(inventory2.slots[slot] == s) << "Incorrect item";

    delete job;
}

TEST_F(Inventory2Test, testMode0Btn0T8){
    Slot s = getStone();
    Slot h = Slot();
    int slot = 0;

    ClickWindowJob* job = initJob(0, slot);
    job->button = 0;

    inventory2.slots[slot] = s;
    inventory2.hover = h;

    inventory2.clickWindow(job, 0, altered, false);

    ASSERT_TRUE(inventory2.hover == s) << "hover: " << inventory2.hover;
    ASSERT_TRUE(inventory2.slots[slot] == h) << "slot: " << inventory2.slots[slot];

    delete job;
}


// items both sides
TEST_F(Inventory2Test, testMode0Btn0T9){
    testMode0Btn0(getStone(), getDirt());
}

TEST_F(Inventory2Test, testMode0Btn0T10){
    testMode0Btn0(getDirt(), getStone());
}

// Single item
TEST_F(Inventory2Test, testMode0Btn1T1){
    testMode0Btn1(getStone(), Slot());
}

TEST_F(Inventory2Test, testMode0Btn1T2){
    testMode0Btn1(Slot(), getStone());
}

// multiple items
TEST_F(Inventory2Test, testMode0Btn1T3){
    testMode0Btn1(getStone(12), Slot());
}

TEST_F(Inventory2Test, testMode0Btn1T4){
    testMode0Btn1(getStone(11), Slot());
}

// Place single item
TEST_F(Inventory2Test, testMode0Btn1T5){
    testMode0Btn1(Slot(), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T6){
    testMode0Btn1(Slot(), getStone(11));
}

// mixed item types
TEST_F(Inventory2Test, testMode0Btn1T7){
    testMode0Btn1(getDirt(), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T8){
    testMode0Btn1(getDirt(15), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T9){
    testMode0Btn1(getDirt(64), getStone(12));
}

// same item both sides
TEST_F(Inventory2Test, testMode0Btn1T10){
    testMode0Btn1(getStone(), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T11){
    testMode0Btn1(getStone(15), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T12){
    testMode0Btn1(getDirt(64), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T13){
    testMode0Btn1(getDirt(64), getStone());
}

TEST_F(Inventory2Test, testMode0Btn1T14){
    testMode0Btn1(getDirt(30), getStone(64));
}


// shift clicks

// inventory to empty hotbar
TEST_F(Inventory2Test, testMode1T1){
    // Set up inventory
    inventory2.slots[9] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[36] == sExpect2) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect2;

    delete job;
}

// hotbar to empty inventory
TEST_F(Inventory2Test, testMode1T2){
    // Set up inventory
    inventory2.slots[36] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;

    delete job;
}

// Hotbar to inventory with dirt in 1st slot
TEST_F(Inventory2Test, testMode1T3){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[9] = getDirt();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getDirt();
    Slot sExpect3 = getStone();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[10] == sExpect3) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect3;

    delete job;
}

// Hotbar to inventory that has a partial stack
TEST_F(Inventory2Test, testMode1T4){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[14] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = Slot();
    Slot sExpect3 = getStone(2);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[14] == sExpect3) << "slot14: " << inventory2.slots[14] << ", expected: " << sExpect3;

    delete job;
}

// Hotbar to inventory that has a full stack
TEST_F(Inventory2Test, testMode1T5){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[14] = getStone(64);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();
    Slot sExpect3 = getStone(64);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[14] == sExpect3) << "slot14: " << inventory2.slots[14] << ", expected: " << sExpect3;

    delete job;
}

// Hotbar to inventory that has a full stack
TEST_F(Inventory2Test, testMode1T6){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[14] = getStone(64);
    inventory2.slots[12] = getStone(4);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone(5);
    Slot sExpect3 = getStone(64);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[12] == sExpect2) << "slot12: " << inventory2.slots[12] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[14] == sExpect3) << "slot14: " << inventory2.slots[14] << ", expected: " << sExpect3;

    delete job;
}

// Hotbar to inventory that has an almost-full stack
TEST_F(Inventory2Test, testMode1T7){
    // Set up inventory
    inventory2.slots[36] = getStone(8);
    inventory2.slots[12] = getStone(60);
    inventory2.slots[14] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone(64);
    Slot sExpect3 = getStone(5);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[12] == sExpect2) << "slot12: " << inventory2.slots[12] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[14] == sExpect3) << "slot14: " << inventory2.slots[14] << ", expected: " << sExpect3;

    delete job;
}

TEST_F(Inventory2Test, testMode1T8){
    // Set up inventory
    inventory2.slots[36] = getStone();

    for(int i=9; i < 36; i++){
        inventory2.slots[i] = getDirt();
    }

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;

    for(int i=9; i<36; i++){
        ASSERT_TRUE(inventory2.slots[i] == sExpect2) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect2;
    }

    delete job;
}

TEST_F(Inventory2Test, testMode1T9){
    // Set up inventory
    inventory2.slots[9] = getStone();

    for(int i=36; i < 45; i++){
        inventory2.slots[i] = getDirt();
    }

    // Create job
    ClickWindowJob* job = initJob(1, 9);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;

    for(int i=36; i<45; i++){
        ASSERT_TRUE(inventory2.slots[i] == sExpect2) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect2;
    }

    delete job;
}

// Hotbar to inventory, must split between two near-full stacks
TEST_F(Inventory2Test, testMode1T10){
    // Set up inventory
    inventory2.slots[36] = getStone(4);
    inventory2.slots[14] = getStone(62);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone(2);
    Slot sExpect3 = getStone(64);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[36] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[9] == sExpect2) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[14] == sExpect3) << "slot14: " << inventory2.slots[14] << ", expected: " << sExpect3;

    delete job;
}

// MODE 2: numeric keys

// move item away
TEST_F(Inventory2Test, testMode2T1){
    //Set up inventory
    inventory2.slots[37] = getStone();

    ClickWindowJob* job = initJob(2, 37);
    job->button = 4;

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[37] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// move item in
TEST_F(Inventory2Test, testMode2T2){
    //Set up inventory
    inventory2.slots[37] = getStone();

    ClickWindowJob* job = initJob(2, 40);
    job->button = 1;

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[37] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// swap hotbar items
TEST_F(Inventory2Test, testMode2T3){
    //Set up inventory
    inventory2.slots[37] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 40);
    job->button = 1;

    Slot hExpect = Slot();
    Slot sExpect1 = getDirt();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[37] == sExpect1) << "slot36: " << inventory2.slots[36] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// swap from main inventory
TEST_F(Inventory2Test, testMode2T4){
    //Set up inventory
    inventory2.slots[10] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 10);
    job->button = 4;

    Slot hExpect = Slot();
    Slot sExpect1 = getDirt();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[10] == sExpect1) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[40] == sExpect2) << "slot40: " << inventory2.slots[40] << ", expected: " << sExpect2;

    delete job;
}

// middle click in survival
TEST_F(Inventory2Test, testMode3T1){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3);
    job->button = 2;

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();

    inventory2.clickWindow(job, 0, altered, false);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;

    delete job;
}

// middle click in creative
TEST_F(Inventory2Test, testMode3T2){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3);
    job->button = 2;

    Slot hExpect = getStone(64);
    Slot sExpect1 = getStone();

    inventory2.clickWindow(job, 0, altered, true);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;

    delete job;
}

// simple mouse drag
TEST_F(Inventory2Test, testMode5T1){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    job->slotNum = 9;
    job->button = 1;
    inventory2.clickWindow(job, 0, altered, false);

    job->slotNum = 10;
    job->button = 1;
    inventory2.clickWindow(job, 0, altered, false);

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone(32);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[10] == sExpect1) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect1;

    delete job;
}

// simple mouse drag (3 slots)
TEST_F(Inventory2Test, testMode5T2){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<12; i++){
        job->slotNum = i;
        job->button = 1;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = getStone();
    Slot sExpect1 = getStone(21);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=9; i<12; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    delete job;
}

// mouse drag, crosses other item
TEST_F(Inventory2Test, testMode5T3){
    //Set up inventory
    inventory2.hover = getStone(64);
    inventory2.slots[10] = getDirt();

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<12; i++){
        job->slotNum = i;
        job->button = 1;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone(32);
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[11] == sExpect1) << "slot11: " << inventory2.slots[11] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[10] == sExpect2) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect1;

    delete job;
}

// mouse drag with too few items
TEST_F(Inventory2Test, testMode5T4){
    //Set up inventory
    inventory2.hover = getStone(3);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 1;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();
    Slot sExpect2 = Slot();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=9; i<12; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[12] == sExpect2) << "slot12: " << inventory2.slots[12] << ", expected: " << sExpect2;
    ASSERT_TRUE(inventory2.slots[13] == sExpect2) << "slot13: " << inventory2.slots[13] << ", expected: " << sExpect2;

    delete job;
}

// simple right mouse drag
TEST_F(Inventory2Test, testMode5T5){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    job->slotNum = 9;
    job->button = 5;
    inventory2.clickWindow(job, 0, altered, false);

    job->slotNum = 10;
    job->button = 5;
    inventory2.clickWindow(job, 0, altered, false);

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = getStone(62);
    Slot sExpect1 = getStone(1);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    ASSERT_TRUE(inventory2.slots[9] == sExpect1) << "slot9: " << inventory2.slots[9] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[10] == sExpect1) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect1;

    delete job;
}

// simple mouse drag (5 slots)
TEST_F(Inventory2Test, testMode5T6){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = getStone(59);
    Slot sExpect1 = getStone(1);

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=9; i<14; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;

    delete job;
}

// right mouse drag with too few items
TEST_F(Inventory2Test, testMode5T7){
    //Set up inventory
    inventory2.hover = getStone(4);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = Slot();
    Slot sExpect1 = getStone(1);
    Slot sExpect2 = Slot();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=9; i<13; i++)
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;
    ASSERT_TRUE(inventory2.slots[13] == sExpect2) << "slot13: " << inventory2.slots[13] << ", expected: " << sExpect2;

    delete job;
}

// simple mouse drag (5 slots)
TEST_F(Inventory2Test, testMode5T8){
    //Set up inventory
    inventory2.hover = getStone(64);
    inventory2.slots[10] = getDirt();

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = getStone(60);
    Slot sExpect1 = getStone();
    Slot sExpect2 = getDirt();

    // Assertions
    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=9; i<14; i++){
        if(i != 10)
            ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;
        else
            ASSERT_TRUE(inventory2.slots[10] == sExpect2) << "slot10: " << inventory2.slots[10] << ", expected: " << sExpect2;
    }

    delete job;
}

// Normal double click (incl. preceding single-click)
TEST_F(Inventory2Test, testMode6T1){
    //Set up inventory
    inventory2.hover = Slot();
    for(int i=9; i<19; i++)
        inventory2.slots[i] = getStone();

    // left click
    ClickWindowJob* job = initJob(0, 9);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    job->mode = 6;
    inventory2.clickWindow(job, 0, altered, false);

    Slot hExpect = getStone(10);
    Slot sExpect1 = Slot();

    ASSERT_TRUE(inventory2.hover == hExpect) << "hover: " << inventory2.hover << ", expected: " << hExpect;
    for(int i=0; i<45; i++){
        ASSERT_TRUE(inventory2.slots[i] == sExpect1) << "slot: " << inventory2.slots[i] << ", expected: " << sExpect1;
    }


    delete job;
}
