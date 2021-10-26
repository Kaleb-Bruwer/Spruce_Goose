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
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(17);

    // get product
    Slot product = craft->getProduct(f);
    cout << product << endl;

    // Verify product
};
