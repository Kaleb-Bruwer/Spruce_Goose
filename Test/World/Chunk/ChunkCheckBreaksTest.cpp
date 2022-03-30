#include<gtest/gtest.h>

#include "../../../src/World/Chunk/Chunk.h"
#include "../../../src/Inventory/BlockData/Furnace.h"
#include "../../../src/Inventory/BlockData/BaseChest.h"

using namespace std;

class ChunkCheckBreaksTest : public ::testing::Test{
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

/*
These tests assume that chunk.breakTicksLeft function as intended
If this function is broken, it would be picked up by other test suites
*/

TEST_F(ChunkCheckBreaksTest, decrement1){
    // Number of remaining ticks must go down

    chunk.startBreak(10, Coordinate<int>(1,1,1));
    int before = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    chunk.checkBreaks();

    int after = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(after + 1 == before) << "Ticks left didn't decrement properly";
}

TEST_F(ChunkCheckBreaksTest, decrement2){
    // Number of remaining ticks must go down

    chunk.startBreak(10, Coordinate<int>(1,1,1));
    int before = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    chunk.checkBreaks();
    chunk.checkBreaks();

    int after = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(after + 2 == before) << "Ticks left didn't decrement properly";
}

TEST_F(ChunkCheckBreaksTest, decrementMultiple){
    // Number of remaining ticks must go down

    chunk.startBreak(10, Coordinate<int>(1,1,1));
    chunk.startBreak(8, Coordinate<int>(1,1,2));
    int before1 = chunk.breakTicksLeft(Coordinate<int>(1,1,1));
    int before2 = chunk.breakTicksLeft(Coordinate<int>(1,1,2));

    chunk.checkBreaks();

    int after1 = chunk.breakTicksLeft(Coordinate<int>(1,1,1));
    int after2 = chunk.breakTicksLeft(Coordinate<int>(1,1,2));

    ASSERT_TRUE(after1 + 1 == before1) << "Ticks left didn't decrement properly";
    ASSERT_TRUE(after2 + 1 == before2) << "Ticks left didn't decrement properly";
}

TEST_F(ChunkCheckBreaksTest, decrementToZero){
    // Number of remaining ticks must go down

    chunk.startBreak(1, Coordinate<int>(1,1,1));
    int before = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    chunk.checkBreaks();

    int after = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(after + 1 == before) << "Ticks left didn't decrement properly";
}

TEST_F(ChunkCheckBreaksTest, decrementBeyondZero){
    // Number of remaining ticks must go down

    chunk.startBreak(1, Coordinate<int>(1,1,1));
    int before = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();

    int after = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(after + 3 == before) << "Ticks left didn't decrement properly";
}

TEST_F(ChunkCheckBreaksTest, almostOverdue){
    // Number of remaining ticks must go down

    chunk.startBreak(1, Coordinate<int>(1,1,1));
    int before = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();

    int after = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(after + 6 == before) << "Ticks left didn't decrement properly";
}

TEST_F(ChunkCheckBreaksTest, removeOverdue){
    // Number of remaining ticks must go down

    chunk.startBreak(1, Coordinate<int>(1,1,1));
    int before = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();
    chunk.checkBreaks();

    int after = chunk.breakTicksLeft(Coordinate<int>(1,1,1));

    ASSERT_TRUE(after == INT_MAX) << "Overdue break not removed: " << after;
}
