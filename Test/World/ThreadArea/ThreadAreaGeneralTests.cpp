#include "ThreadAreaTestHelpers.h"

#include "../../../src/Protocol/PlayerConnection1_7.h"
#include "../../../src/World/World.h"
#include "../../../src/JobTickets/WorldToWorld/ChunkToThreadArea.h"
#include "../../../src/JobTickets/ProtocolToWorld/ClickWindowJob.h"

#include "../EntityStore/ESHelpers.cpp"

using namespace std;

/*
split() never actually accesses World or SynchedArea, it only copies the ptrs
So completely fake values that were typecast should work fine
*/

class ThreadAreaGeneralTests : public ::testing::Test{
protected:
    // false disables creation of its own thread
    World* world = (World*) 0x01;
    ThreadArea tArea = ThreadArea(false);

    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

};

TEST_F(ThreadAreaGeneralTests, checkDisconnects){

    const int numPlayers = 3;
    PlayerConnection* conn[numPlayers];
    PlayerEntity* players[numPlayers];

    for(int i=0; i < numPlayers; i++){
        // 1. Construct players with PlayerConnections
        conn[i] = new PlayerConnection1_7(0, world);
        players[i] = new PlayerEntity(conn[i]);
        // 2. Add players to ThreadArea
        conn[i]->addThreadArea(&tArea);
        tArea.addEntity(players[i]);
    }


    // 3. Mark players to quit
    for(int i=0; i<numPlayers-1; i++){
        conn[i]->quit = true;
    }

    // 4. Check for disconnects
    tArea.checkForDisconnects();

    // 5. Verify
    for(int i=0; i<numPlayers-1; i++){
        Entity* res = tArea.getEntity(players[i]->getEid());
        ASSERT_TRUE(res == 0) << "quit player wasn't removed " << i;
    }
    Entity* res = tArea.getEntity(players[numPlayers-1]->getEid());
    ASSERT_TRUE(res == players[numPlayers-1]) << "wrong player was removed";

}

TEST_F(ThreadAreaGeneralTests, includeChunk){
    SynchedArea* s = new ns_ta::SynchedArea_mock(0, true);
    ThreadArea tArea2 = ThreadArea(s, 0, false);

    // Prepare threadArea
    tAreaClaimChunks(tArea2, vector<ChunkCoord>{ChunkCoord(1,1)});

    // Send job
    ChunkToThreadArea* job = new ChunkToThreadArea();

    // Needs to be a real chunk
    Chunk* c = new Chunk(1,1);
    job->chunk = c;


    tArea2.includeChunk(job);

    // Assert that chunk was included

    verifyChunks(tArea2, vector<ChunkCoord>{ChunkCoord(1,1)},
            vector<Chunk*>{c});

    delete s;
}

TEST_F(ThreadAreaGeneralTests, includeChunkPlayers){
    SynchedArea* s = new ns_ta::SynchedArea_mock(0, true);
    ThreadArea tArea2 = ThreadArea(s, 0, false);

    // Prepare threadArea
    tAreaClaimChunks(tArea2, vector<ChunkCoord>{ChunkCoord(1,1)});

    // Add player
    ns_es::DummyPlayer* player = new ns_es::DummyPlayer(101);
    tArea2.entities.addPlayerEntity(player);

    // Send job
    ChunkToThreadArea* job = new ChunkToThreadArea();

    // Needs to be a real chunk
    Chunk* c = new Chunk(1,1);
    job->chunk = c;


    tArea2.includeChunk(job);

    // Assert that chunk was included

    ASSERT_TRUE(c->hasPlayers());

    delete s;
    delete player;
}

TEST_F(ThreadAreaGeneralTests, includeChunkRedirect){
    World world(false);
    SynchedArea* s = new ns_ta::SynchedArea_mock(&world, false);
    ThreadArea tArea2 = ThreadArea(s, &world, false);

    // Send job
    ChunkToThreadArea* job = new ChunkToThreadArea();

    // Needs to be a real chunk
    Chunk* c = new Chunk(1,1);
    job->chunk = c;

    tArea2.includeChunk(job);

    // Assert that chunk was included

    ASSERT_TRUE(world.inQueue.size() == 1) << "World didn't receive redirect";

    delete s;
}

TEST_F(ThreadAreaGeneralTests, clickWindow){
    // Set up player + inventory
    PlayerConnection* conn = new PlayerConnection1_7(0, 0);
    PlayerEntity* player = new PlayerEntity(conn);
    player->position = Coordinate<double>(1,70,1);

    player->inventory.inventory.slots[9] = Slot(1);

    // Add player to tArea
    tArea.entities.addPlayerEntity(player);

    // Construct and send requests
    // Mode 4, Button 0: (Drop key)
    ClickWindowJob* job = new ClickWindowJob();
    job->mode = 4;
    job->button = 0;
    job->slotNum = 9;

    tArea.clickWindowHandler(job, player);

    // Verify item was dropped
    ns_ta::CountingFunctor func;
    tArea.entities.executeFunctorAll(func);
    // Player + dropped item
    ASSERT_TRUE(func.counter == 2) << "Item wasn't created";

    delete player;
    delete conn;
}
