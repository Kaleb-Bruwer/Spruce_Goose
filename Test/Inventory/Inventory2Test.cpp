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

    ClickWindowJob* initJob(int slotNum = 9){
        ClickWindowJob* result = new ClickWindowJob();

        result->windowID = 0;
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
        ClickWindowJob* job = initJob(slot);
        job->mode = 0;
        job->button = 0;

        inventory2.slots[slot] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);

        ASSERT_TRUE(inventory2.hover == s) << "Incorrect item";
        ASSERT_TRUE(inventory2.slots[slot] == h) << "Incorrect item";
    }

    void testMode0Btn1(Slot s, Slot h, int slot = 9){
        ClickWindowJob* job = initJob(slot);
        job->mode = 0;
        job->button = 1;

        inventory2.slots[slot] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);

        Slot sExpect;
        Slot hExpect;
        // If hover is empty, pick up half of slot with hover getting +1 on odds
        if(inventory2.hover.isEmpty()){
            int items = ceil( ((float)s.itemCount) / 2);
            hExpect = s;
            hExpect.itemCount = items;
            sExpect.itemCount -= items;
            if(sExpect.itemCount == 0){
                sExpect = Slot();
            }
        }
        else if(s.itemID == h.itemID){
            if(s.itemCount < s.maxStackSize()){
                sExpect = s;
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

        ASSERT_TRUE(inventory2.hover == hExpect) << "Incorrect item";
        ASSERT_TRUE(inventory2.slots[slot] == sExpect) << "Incorrect item";
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

    ClickWindowJob* job = initJob(slot);
    job->mode = 0;
    job->button = 0;

    inventory2.slots[slot] = s;
    inventory2.hover = h;

    inventory2.clickWindow(job, 0, altered, false);

    ASSERT_TRUE(inventory2.hover == h) << "Incorrect item";
    ASSERT_TRUE(inventory2.slots[slot] == s) << "Incorrect item";
}

TEST_F(Inventory2Test, testMode0Btn0T8){
    Slot s = getStone();
    Slot h = Slot();
    int slot = 0;

    ClickWindowJob* job = initJob(slot);
    job->mode = 0;
    job->button = 0;

    inventory2.slots[slot] = s;
    inventory2.hover = h;

    inventory2.clickWindow(job, 0, altered, false);

    ASSERT_TRUE(inventory2.hover == s) << "hover: " << inventory2.hover;
    ASSERT_TRUE(inventory2.slots[slot] == h) << "slot: " << inventory2.slots[slot];
}


// items both sides
TEST_F(Inventory2Test, testMode0Btn0T9){
    testMode0Btn0(getStone(), getDirt());
}

TEST_F(Inventory2Test, testMode0Btn0T10){
    testMode0Btn0(getDirt(), getStone());
}
