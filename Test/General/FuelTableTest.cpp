#include <gtest/gtest.h>
#include <vector>

#include "../../src/General/FuelTable.h"


class FuelTableTest : public ::testing::Test{
    private:
    virtual void SetUp(){
    };

    virtual void TearDown(){

    };

};


TEST_F(FuelTableTest, test1){
    FuelTable* ft = FuelTable::getInstance();

    int duration = ft->getDuration(263);
    ASSERT_TRUE(duration == 1600) << "Incorrect burn duration: " << duration;

    duration = ft->getDuration(327);
    ASSERT_TRUE(duration == 20000) << "Incorrect burn duration: " << duration;
}
