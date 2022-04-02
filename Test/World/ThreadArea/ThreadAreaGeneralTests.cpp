#include "ThreadAreaTestHelpers.h"

#include "../../../src/Protocol/PlayerConnection1_7.h"

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
