#include<gtest/gtest.h>

#include "../../../src/World/Chunk/Chunk.h"
#include "../../../src/Inventory/BlockData/Furnace.h"
#include "../../../src/Inventory/BlockData/BaseChest.h"

using namespace std;

class ChunkBreakBlockTest : public ::testing::Test{
protected:
    Chunk chunk = Chunk(0,0);

    const Slot diamondPick = Slot(278);
    const Slot diamondShovel = Slot(277);
    const Slot diamondAxe = Slot(279);

    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

    // checks if correct items are contained in drops, regardless of order
    void assertDrops(vector<Item*> drops, vector<Slot> expected){
        ASSERT_TRUE(drops.size() == expected.size())
            << "wrong amount of drops: " << drops.size() << " instead of " << expected.size();

        vector<bool> used(drops.size(), false);

        for(int i=0; i<expected.size(); i++){
            bool found = false;
            for(int j=0; j<drops.size(); j++){
                if(used[j])
                    continue;
                if((expected[i].itemID == drops[j]->itemID)
                        && (expected[i].itemCount == drops[j]->count)
                        && (expected[i].itemDamage == drops[j]->itemDamage)){
                    found = true;
                    used[j] = true;
                    break;
                }
            }
            ASSERT_TRUE(found) << expected[i] << " wasn't dropped\n";
        }
    }
};

TEST_F(ChunkBreakBlockTest, break1){
    // break stone with pickaxe
    chunk.setBlock(Coordinate<int>(1,1,1), Block(1));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondPick);
    ASSERT_TRUE(drops.size() == 1) << "Wrong number of drops returned";

    ASSERT_TRUE(drops[0]->itemID == 4) << "Wrong item dropped";
    ASSERT_TRUE(drops[0]->count == 1) << "Wrong number of items dropped";
}

TEST_F(ChunkBreakBlockTest, break2){
    // break stone with fist
    chunk.setBlock(Coordinate<int>(1,1,1), Block(1));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), Slot());
    ASSERT_TRUE(drops.size() == 0) << "Wrong number of drops returned";
}

TEST_F(ChunkBreakBlockTest, break3){
    // break dirt with shovel
    chunk.setBlock(Coordinate<int>(1,1,1), Block(3));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondShovel);
    ASSERT_TRUE(drops.size() == 1) << "Wrong number of drops returned";

    ASSERT_TRUE(drops[0]->itemID == 3) << "Wrong item dropped";
    ASSERT_TRUE(drops[0]->count == 1) << "Wrong number of items dropped";
}


TEST_F(ChunkBreakBlockTest, breakFurnace1){
    // break a furnace (with content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(61));
    Furnace* furnace = (Furnace*) chunk.getBlockData(Coordinate<int>(1,1,1));
    furnace->slots[1] = Slot(1);

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondPick);

    assertDrops(drops, vector<Slot>{Slot(1), Slot(61)});
}

TEST_F(ChunkBreakBlockTest, breakFurnace2){
    // break a furnace (without content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(61));
    Furnace* furnace = (Furnace*) chunk.getBlockData(Coordinate<int>(1,1,1));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondPick);

    assertDrops(drops, vector<Slot>{Slot(61)});
}

TEST_F(ChunkBreakBlockTest, breakFurnace3){
    // break a furnace (with content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(61));
    Furnace* furnace = (Furnace*) chunk.getBlockData(Coordinate<int>(1,1,1));
    furnace->slots[0] = Slot(1);
    furnace->slots[1] = Slot(263);

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondPick);

    assertDrops(drops, vector<Slot>{Slot(61), Slot(1), Slot(263)});
}

TEST_F(ChunkBreakBlockTest, breakChest1){
    // break a chest (without content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    ChestSingle* chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    // chest->slots[1] = Slot(1);

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    assertDrops(drops, vector<Slot>{Slot(54)});
}

TEST_F(ChunkBreakBlockTest, breakChest2){
    // break a chest (with content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    ChestSingle* chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    chest->slots[1] = Slot(1);

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    assertDrops(drops, vector<Slot>{Slot(54), Slot(1)});
}

TEST_F(ChunkBreakBlockTest, breakChest3){
    // break a chest (check blockData)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    ChestSingle* chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    chest->slots[1] = Slot(1);

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);


    chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    ASSERT_TRUE(chest == 0) << "Chest BlockData wasn't deleted";

}

TEST_F(ChunkBreakBlockTest, breakDoubleChest1){
    // break a double chest (without content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    ChestSingle* chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    ASSERT_TRUE(chest == 0) << "Broken chest still returned block data";

    chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,2));
    ASSERT_TRUE(chest != 0) << "Intact chest not returned";
    ASSERT_TRUE(chest->getType() == CHESTSINGLE) << "Wrong type blockdata returned";
}

TEST_F(ChunkBreakBlockTest, breakDoubleChest2){
    // break a double chest (without content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,2), diamondAxe);

    ChestSingle* chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,2));
    ASSERT_TRUE(chest == 0) << "Broken chest still returned block data";

    chest = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    ASSERT_TRUE(chest != 0) << "Intact chest not returned";
    ASSERT_TRUE(chest->getType() == CHESTSINGLE) << "Wrong type blockdata returned";
}

TEST_F(ChunkBreakBlockTest, breakDoubleChest3){
    // break a double chest (with content)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));

    ChestDouble* chest = (ChestDouble*) chunk.getBlockData(Coordinate<int>(1,1,1));
    chest->slots[0] = Slot(1);


    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    assertDrops(drops, vector<Slot>{Slot(54), Slot(1)});
}

TEST_F(ChunkBreakBlockTest, breakDoubleChest4){
    // break a double chest (with content in intact half)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));
    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));

    ChestDouble* chest = (ChestDouble*) chunk.getBlockData(Coordinate<int>(1,1,1));
    chest->slots[27] = Slot(1);


    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    assertDrops(drops, vector<Slot>{Slot(54)});
}

TEST_F(ChunkBreakBlockTest, breakDoubleChest5){
    // break a double chest (content started in single chest)
    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));

    ChestSingle* chestSingle = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,1));
    chestSingle->slots[0] = Slot(1);

    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    assertDrops(drops, vector<Slot>{Slot(54), Slot(1)});
}

TEST_F(ChunkBreakBlockTest, breakDoubleChest6){
    // break a double chest (content started in single chest)
    chunk.setBlock(Coordinate<int>(1,1,2), Block(54));
    ChestSingle* chestSingle = (ChestSingle*) chunk.getBlockData(Coordinate<int>(1,1,2));
    chestSingle->slots[0] = Slot(1);

    chunk.setBlock(Coordinate<int>(1,1,1), Block(54));

    vector<Item*> drops = chunk.breakBlock(Coordinate<int>(1,1,1), diamondAxe);

    assertDrops(drops, vector<Slot>{Slot(54)});
}
