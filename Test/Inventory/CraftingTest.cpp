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
    // Oak planks
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(17);

    // get & verify product
    Slot expected(5);
    expected.itemCount = 4;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected);
};

TEST_F(CraftingTest, test1){
    // Spruce planks
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(17,1);

    // get & verify product
    Slot expected(5, 1);
    expected.itemCount = 4;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected);
};

TEST_F(CraftingTest, test2){
    // Crafting table from oak planks
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[0][1] = Slot(5);
    f.frame[1][0] = Slot(5);
    f.frame[1][1] = Slot(5);

    // get & verify product
    Slot expected(58);
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test3){
    // Crafting table from mix of planks
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5,5);
    f.frame[0][1] = Slot(5,2);
    f.frame[1][0] = Slot(5,1);
    f.frame[1][1] = Slot(5,3);

    // get & verify product
    Slot expected(58);
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test4){
    // Superset of valid recipe
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[0][1] = Slot(5);
    f.frame[1][0] = Slot(5);
    f.frame[1][1] = Slot(5);
    f.frame[2][1] = Slot(5);

    // get & verify product
    Slot expected;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test5){
    // Subset of valid recipe
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[0][1] = Slot(5);

    f.frame[1][1] = Slot(5);

    // get & verify product
    Slot expected;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test6){
    // Sticks
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[1][0] = Slot(5);


    // get & verify product
    Slot expected(280);
    expected.itemCount = 4;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test7){
    // Stone pickaxe (heterogenous input test)
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(4);
    f.frame[0][1] = Slot(4);
    f.frame[0][2] = Slot(4);

    f.frame[1][1] = Slot(280);
    f.frame[2][1] = Slot(280);


    // get & verify product
    Slot expected(274);
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test8){
    // cobble staircase
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(4);
    f.frame[1][0] = Slot(4);
    f.frame[1][1] = Slot(4);
    f.frame[2][0] = Slot(4);
    f.frame[2][1] = Slot(4);
    f.frame[2][2] = Slot(4);

    // get & verify product
    Slot expected(67);
    expected.itemCount = 4;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test9){
    // mixed-wood stairs (shouldn't work)
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[1][0] = Slot(5,1);
    f.frame[1][1] = Slot(5,2);
    f.frame[2][0] = Slot(5,3);
    f.frame[2][1] = Slot(5,4);
    f.frame[2][2] = Slot(5,5);

    // get & verify product
    Slot expected;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test10){
    // bow
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][2] = Slot(287);
    f.frame[1][2] = Slot(287);
    f.frame[2][2] = Slot(287);
    f.frame[0][1] = Slot(280);
    f.frame[1][0] = Slot(280);
    f.frame[2][1] = Slot(280);

    // get & verify product
    Slot expected(261);
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test11){
    // empty input
    // Construct crafting frame
    CraftingFrame f(3, 3);

    // get & verify product
    Slot expected;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test12){
    // Sticks, more input
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[0][0].itemCount = 2;
    f.frame[1][0] = Slot(5);
    f.frame[1][0].itemCount = 1;

    // get & verify product
    Slot expected(280);
    expected.itemCount = 4;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test13){
    // Sticks, more input
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[0][0] = Slot(5);
    f.frame[0][0].itemCount = 64;
    f.frame[1][0] = Slot(5);
    f.frame[1][0].itemCount = 63;

    // get & verify product
    Slot expected(280);
    expected.itemCount = 4;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};

TEST_F(CraftingTest, test14){
    // invalid
    // Construct crafting frame
    CraftingFrame f(3, 3);
    f.frame[1][0] = Slot(280);
    f.frame[1][0].itemCount = 63;

    // get & verify product
    Slot expected;
    Slot product = craft->getProduct(f);

    ASSERT_TRUE(product == expected) << product << " found but " << expected << " expected";
};
