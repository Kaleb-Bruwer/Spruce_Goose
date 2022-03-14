#include<gtest/gtest.h>

#include "../../../src/World/Chunk/Chunk.h"

using namespace std;

class ChunkSetBlockTest : public ::testing::Test{
protected:
    Chunk chunk = Chunk(0,0);

    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

    // Cannot handle duplicates
    void setBlockBasic(vector<tuple<Coordinate<int>, Block>> additions){
        int size = additions.size();

        for(int i=0; i<size; i++){
            chunk.setBlock(get<0>(additions[i]), get<1>(additions[i]));
        }

        for(int i=0; i<size; i++){
            Block ret = chunk.getBlock(get<0>(additions[i]));
            ASSERT_TRUE(ret == get<1>(additions[i])) << "Incorrect block returned";
        }
    }

};

TEST_F(ChunkSetBlockTest, setBlock1){
    // Simple block
    setBlockBasic(vector<tuple<Coordinate<int>, Block>>{
        make_tuple(Coordinate<int>(1,4,8), Block(3))
    });
};

TEST_F(ChunkSetBlockTest, setBlock2){
    // Simple block
    setBlockBasic(vector<tuple<Coordinate<int>, Block>>{
        make_tuple(Coordinate<int>(1,99,8), Block(3))
    });
};

TEST_F(ChunkSetBlockTest, setBlock3){
    // Multiple blocks
    setBlockBasic(vector<tuple<Coordinate<int>, Block>>{
        make_tuple(Coordinate<int>(1,99,8), Block(3)),
        make_tuple(Coordinate<int>(4,6,12), Block(12))
    });
};

TEST_F(ChunkSetBlockTest, setBlockData1){
    // Set block that has blockdata

    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));

    Block ret = chunk.getBlock(Coordinate<int>(1,1,1));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    BlockData* d = chunk.getBlockData(Coordinate<int>(1,1,1));
    ASSERT_TRUE(d != 0) << "Didn't return blockData";

    ASSERT_TRUE(d->getType() == CHESTSINGLE) << "returned wrong blockData";

}

TEST_F(ChunkSetBlockTest, setBlockData2){
    // Set block that has blockdata

    chunk.setBlock(Coordinate<int>(1,1,1), Block(61));

    Block ret = chunk.getBlock(Coordinate<int>(1,1,1));
    ASSERT_TRUE(ret == Block(61)) << "Returned wrong (or no) block";

    BlockData* d = chunk.getBlockData(Coordinate<int>(1,1,1));
    ASSERT_TRUE(d != 0) << "Didn't return blockData";

    ASSERT_TRUE(d->getType() == FURNACE) << "returned wrong blockData";
}

TEST_F(ChunkSetBlockTest, setDoubleChest1){
    // Set double chest and check return values

    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    chunk.setBlock(Coordinate<int>(2,1,1), Block(54));

    Block ret = chunk.getBlock(Coordinate<int>(1,1,1));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    ret = chunk.getBlock(Coordinate<int>(2,1,1));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    BlockData* blockData1 = chunk.getBlockData(Coordinate<int>(1,1,1));
    BlockData* blockData2 = chunk.getBlockData(Coordinate<int>(2,1,1));
    ASSERT_TRUE(blockData1 != 0) << "Didn't return blockData";
    ASSERT_TRUE(blockData2 != 0) << "Didn't return blockData";


    ASSERT_TRUE(blockData1->getType() == CHESTDOUBLE) << "returned wrong blockData";
    ASSERT_TRUE(blockData1 == blockData2) << "returned different chests";
}

TEST_F(ChunkSetBlockTest, setDoubleChest2){
    // Set double chest and check return values

    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));

    Block ret = chunk.getBlock(Coordinate<int>(1,1,1));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    ret = chunk.getBlock(Coordinate<int>(1,1,2));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    BlockData* blockData1 = chunk.getBlockData(Coordinate<int>(1,1,1));
    BlockData* blockData2 = chunk.getBlockData(Coordinate<int>(1,1,2));
    ASSERT_TRUE(blockData1 != 0) << "Didn't return blockData";
    ASSERT_TRUE(blockData2 != 0) << "Didn't return blockData";


    ASSERT_TRUE(blockData1->getType() == CHESTDOUBLE) << "returned wrong blockData";
    ASSERT_TRUE(blockData1 == blockData2) << "returned different chests";
}

TEST_F(ChunkSetBlockTest, setDoubleChest3){
    // Set double chest and check return values

    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));

    Block ret = chunk.getBlock(Coordinate<int>(1,1,1));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    ret = chunk.getBlock(Coordinate<int>(1,1,2));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    BlockData* blockData1 = chunk.getBlockData(Coordinate<int>(1,1,1));
    BlockData* blockData2 = chunk.getBlockData(Coordinate<int>(1,1,2));
    ASSERT_TRUE(blockData1 != 0) << "Didn't return blockData";
    ASSERT_TRUE(blockData2 != 0) << "Didn't return blockData";


    ASSERT_TRUE(blockData1->getType() == CHESTDOUBLE) << "returned wrong blockData";
    ASSERT_TRUE(blockData1 == blockData2) << "returned different chests";
}

TEST_F(ChunkSetBlockTest, setDoubleChest4){
    // Place chest next to furnace (negative case)

    chunk.setBlock(Coordinate<int>(1,1,1), Block(61));
    chunk.setBlock(Coordinate<int>(2,1,1), Block(54));

    Block ret = chunk.getBlock(Coordinate<int>(1,1,1));
    ASSERT_TRUE(ret == Block(61)) << "Returned wrong (or no) block";

    ret = chunk.getBlock(Coordinate<int>(2,1,1));
    ASSERT_TRUE(ret == Block(54)) << "Returned wrong (or no) block";

    BlockData* blockData1 = chunk.getBlockData(Coordinate<int>(1,1,1));
    BlockData* blockData2 = chunk.getBlockData(Coordinate<int>(2,1,1));
    ASSERT_TRUE(blockData1 != 0) << "Didn't return blockData";
    ASSERT_TRUE(blockData2 != 0) << "Didn't return blockData";


    ASSERT_TRUE(blockData1->getType() == FURNACE) << "returned wrong blockData";
    ASSERT_TRUE(blockData2->getType() == CHESTSINGLE) << "returned wrong blockData";
}
