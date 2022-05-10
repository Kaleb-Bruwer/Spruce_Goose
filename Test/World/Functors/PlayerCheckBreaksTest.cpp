#include "../../../src/World/Functors/PlayerCheckBreaksF.h"
#include "../../../src/World/ThreadArea.h"
#include "../../../src/Protocol/PlayerConnection1_7.h"

#include "../ThreadArea/ThreadAreaTestHelpers.h"

class PlayerCheckBreaksTest : public ::testing::Test{
protected:
    PlayerConnection* conn;
    PlayerEntity* player;

    ThreadArea tArea = ThreadArea(false);
    PlayerCheckBreaksF func = PlayerCheckBreaksF(&tArea);

    virtual void SetUp(){
        conn = new PlayerConnection1_7(0, 0);
        player = new PlayerEntity(conn);
    };

    virtual void TearDown(){
        delete player;
        delete conn;
    };
};

TEST_F(PlayerCheckBreaksTest, reduceCooldown){
    player->isBreaking = false;
    player->breakCooldown = 3;

    func(player);

    ASSERT_TRUE(player->breakCooldown == 2) << "BreakCooldown wasn't properly adjusted";
}

TEST_F(PlayerCheckBreaksTest, startDig){
    player->isBreaking = false;
    player->breakCooldown = 0;
    player->doneBreaking = true;

    Chunk* c = new Chunk(0,0);
    c->setBlock(Coordinate<int>(5,5,5), Block(1));
    player->breakTarget = Coordinate<int>(5,5,5);

    tAreaSetChunks(tArea, vector<ChunkCoord>{ChunkCoord(0,0)}, vector<Chunk*>{c});

    func(player);

    ASSERT_TRUE(player->isBreaking) << "Dig didn't start";
    ASSERT_TRUE(!player->doneBreaking) << "Dig still marked as done";
}

TEST_F(PlayerCheckBreaksTest, endDig){
    player->isBreaking = true;
    player->breakCooldown = 0;
    player->doneBreaking = true;

    Chunk* c = new Chunk(0,0);
    c->setBlock(Coordinate<int>(5,5,5), Block(3)); //dirt
    player->breakTarget = Coordinate<int>(5,5,5);

    tAreaSetChunks(tArea, vector<ChunkCoord>{ChunkCoord(0,0)}, vector<Chunk*>{c});

    func(player);

    ASSERT_TRUE(!player->isBreaking) << "Dig didn't end";
    ASSERT_TRUE(!player->doneBreaking) << "Dig still marked as done";
}
