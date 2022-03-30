#include<gtest/gtest.h>

#include "../../../src/World/Chunk/Chunk.h"
#include "../../../src/Inventory/BlockData/Furnace.h"
#include "../../../src/Inventory/BlockData/BaseChest.h"

using namespace std;

class ChunkStartBreakTest : public ::testing::Test{
protected:
    Chunk chunk = Chunk(0,0);

    const Slot diamondPick = Slot(278);
    const Slot diamondShovel = Slot(277);
    const Slot diamondAxe = Slot(279);

    virtual void SetUp(){
        chunk.setBlock(Coordinate<int>(1,1,1), Block(1));
        chunk.setBlock(Coordinate<int>(1,1,2), Block(1));
        chunk.setBlock(Coordinate<int>(1,1,3), Block(1));
        chunk.setBlock(Coordinate<int>(1,1,4), Block(1));
    };

    virtual void TearDown(){

    };

};

TEST_F(ChunkStartBreakTest, default){
    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(bLeft == INT_MAX) << "Wrong value on default case";
}

TEST_F(ChunkStartBreakTest, dur1){
    chunk.startBreak(10, Coordinate<int>(1,1,1));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(bLeft == 10) << "Wrong breakTicksLeft value";
}

TEST_F(ChunkStartBreakTest, dur2){
    chunk.startBreak(1, Coordinate<int>(1,1,1));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(bLeft == 1) << "Wrong breakTicksLeft value";
}

TEST_F(ChunkStartBreakTest, instant){
    chunk.startBreak(0, Coordinate<int>(1,1,1));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(bLeft == INT_MAX) << "Insta-mine treated as normal break";
}

TEST_F(ChunkStartBreakTest, multi1){
    chunk.startBreak(4, Coordinate<int>(1,1,1));
    chunk.startBreak(8, Coordinate<int>(1,1,2));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));
    ASSERT_TRUE(bLeft == 4) << "Wrong breakTicksLeft value";

    bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,2));
    ASSERT_TRUE(bLeft == 8) << "Wrong breakTicksLeft value";
}

TEST_F(ChunkStartBreakTest, multi2){
    chunk.startBreak(4, Coordinate<int>(1,1,1));
    chunk.startBreak(0, Coordinate<int>(1,1,2));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));
    ASSERT_TRUE(bLeft == 4) << "Wrong breakTicksLeft value";

    bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,2));
    ASSERT_TRUE(bLeft == INT_MAX) << "Insta-mine treated as normal break";
}

TEST_F(ChunkStartBreakTest, duplicate1){
    // Server uses faster of the two simultaneous breaks
    chunk.startBreak(4, Coordinate<int>(1,1,1));
    chunk.startBreak(1, Coordinate<int>(1,1,1));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));
    ASSERT_TRUE(bLeft == 1) << "Wrong breakTicksLeft value";
}

TEST_F(ChunkStartBreakTest, duplicate2){
    // Instamine cancels pending breaks
    chunk.startBreak(4, Coordinate<int>(1,1,1));
    chunk.startBreak(0, Coordinate<int>(1,1,1));

    int bLeft = chunk.breakTicksLeft(Coordinate<int>(1,1,1));
    ASSERT_TRUE(bLeft == INT_MAX) << "Wrong breakTicksLeft value";
}
