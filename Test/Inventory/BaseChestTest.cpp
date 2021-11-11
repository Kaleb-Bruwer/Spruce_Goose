#include<gtest/gtest.h>
#include <cmath>

#include "CommonHelpers.cpp"

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
};

TEST_F(BaseChestTest, combineSingleChests){
    
}
