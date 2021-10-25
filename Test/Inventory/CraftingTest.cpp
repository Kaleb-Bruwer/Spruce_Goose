#include<gtest/gtest.h>

#include "../../src/Inventory/Crafting/Crafting.h"

class CraftingTest : public ::testing::Test{
protected:
    inline static Crafting* craft;

    static void SetUpTestCase(){
        craft = Crafting::getInstance();
    };

};

TEST_F(CraftingTest, test0){
};
