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

    ClickWindowJob* initJob(){
        ClickWindowJob* result = new ClickWindowJob();

        result->windowID = 0;
        result->slotNum = 9;

        return result;
    };

    Slot getStone(){
        Slot result;
        result.itemID = 1;
        result.itemCount = 1;
        return result;
    };
};

TEST_F(Inventory2Test, simplePickup){
    ClickWindowJob* job = initJob();
    job->mode = 0;
    job->button = 0;

    inventory2.slots[9] = getStone();

    inventory2.clickWindow(job, 0, altered, false);



    ASSERT_TRUE(inventory2.hover == getStone()) << "Incorrect item";
    ASSERT_TRUE(inventory2.slots[9] == Slot()) << "Incorrect item";
}

TEST_F(Inventory2Test, simplePutdown){
    ClickWindowJob* job = initJob();
    job->mode = 0;
    job->button = 0;

    inventory2.hover = getStone();

    inventory2.clickWindow(job, 0, altered, false);



    ASSERT_TRUE(inventory2.hover == Slot()) << "Incorrect item";
    ASSERT_TRUE(inventory2.slots[9] == getStone()) << "Incorrect item";
}
