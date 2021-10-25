#include<gtest/gtest.h>

#include "../../src/Inventory/Crafting/CraftingFrame.h"

class CraftingFrameTest : public ::testing::Test{
protected:
    CraftingFrame cf;

    void SetUp(){
        cf = CraftingFrame(3,3);
    };

    void doAssertions(int x, int y){
        ASSERT_TRUE(cf.x == x) << "CraftingFrame over-trimmed, " << cf.x << " should be " << x;
        ASSERT_TRUE(cf.y == y) << "CraftingFrame over-trimmed, " << cf.y << " should be " << y;


        ASSERT_TRUE(cf.y == cf.frame.size()) << "Frame-vector size mismatch";

        for(int i=0; i < cf.y; i++)
            ASSERT_TRUE(cf.x == cf.frame[i].size()) << "Frame-vector size mismatch";
    }

};

TEST_F(CraftingFrameTest, test0){
    cf.frame[0][0] = Slot(1);
    cf.frame[2][2] = Slot(1);

    cf.trim();
    doAssertions(3, 3);
};

TEST_F(CraftingFrameTest, test1){
    cf.frame[0][0] = Slot(1);
    cf.frame[1][1] = Slot(1);

    cf.trim();
    doAssertions(2, 2);
};

TEST_F(CraftingFrameTest, test2){
    cf.frame[0][0] = Slot(1);
    cf.frame[1][2] = Slot(1);

    cf.trim();
    doAssertions(3, 2);
};

TEST_F(CraftingFrameTest, test3){
    cf.frame[0][0] = Slot(1);
    cf.frame[2][1] = Slot(1);

    cf.trim();
    doAssertions(2, 3);
};

TEST_F(CraftingFrameTest, test4){
    cf.frame[1][0] = Slot(1);
    cf.frame[2][2] = Slot(1);

    cf.trim();
    doAssertions(3, 2);
};

TEST_F(CraftingFrameTest, test5){
    cf.frame[1][1] = Slot(1);
    cf.frame[2][2] = Slot(1);

    cf.trim();
    doAssertions(2, 2);
};

TEST_F(CraftingFrameTest, test6){
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            cf = CraftingFrame(3,3);
            cf.frame[i][j] = Slot(1);
            cf.trim();
            doAssertions(1, 1);
        }
    }
};
