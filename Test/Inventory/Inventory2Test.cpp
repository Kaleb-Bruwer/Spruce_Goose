#include<gtest/gtest.h>

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
};

TEST_F(Inventory2Test, testMode0Btn0T1){
    testMode0Btn0(getStone(), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T2){
    testMode0Btn0(Slot(), getStone());
}

TEST_F(Inventory2Test, testMode0Btn0T3){
    testMode0Btn0(getStone(5), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T4){
    testMode0Btn0(Slot(), getStone(5));
}
