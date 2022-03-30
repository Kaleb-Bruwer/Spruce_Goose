#include<gtest/gtest.h>

#include <vector>

#include "../../../src/World/Chunk/ChunkChangeTracker.h"
#include "../../../src/World/Chunk/CompChunkPacket.h"

#include "../../../src/JobTickets/SendPacket.h"

#include "../../../src/Protocol/PlayerConnection1_7.h"

#include "../../../src/Entities/PlayerEntity.h"

using namespace std;

namespace ns_cct{
    class MockChunkChangeTracker : public ChunkChangeTracker{
    public:
        MockChunkChangeTracker(ChunkCoord coord, CompChunkPacket* chunk):
        ChunkChangeTracker(coord, chunk){};
    };


    class MockCompChunkPacket : public CompChunkPacket{
    public:
        MockCompChunkPacket(){
            job = new SendPacket(); //completely empty
            done = true;
        }
    };

    class MockPlayerConnection : public PlayerConnection1_7{
    public:
        vector<JobTicket*> received;

        MockPlayerConnection() : PlayerConnection1_7(0,0){};
        void pushJobToPlayer(JobTicket* job){
            received.push_back(job);
        }
    };

    class MockPlayer : public PlayerEntity{
    public:
        MockPlayer() : PlayerEntity(new MockPlayerConnection()){
            hasSpawned = true;
        }
        ~MockPlayer(){
            delete connection;
            connection = 0;
        }

        MockPlayerConnection* getMockConn(){
            return (MockPlayerConnection*) connection;
        }
    };
}

class ChunkChangeTrackerTest : public ::testing::Test{
protected:
    ChunkChangeTracker* tracker = 0;

    virtual void SetUp(){
        tracker = new ns_cct::MockChunkChangeTracker(ChunkCoord(5,5),
            new ns_cct::MockCompChunkPacket());
    };

    virtual void TearDown(){

    };

};

TEST_F(ChunkChangeTrackerTest, receiveChunk){
    ns_cct::MockPlayer* p = new ns_cct::MockPlayer();

    tracker->addPlayer(p);
    tracker->sendChanges();

    vector<JobTicket*> received = p->getMockConn()->received;
    int nPackets = received.size();


    ASSERT_TRUE(nPackets == 1)
        << "Player received " << nPackets << " packets instead of 1\n";

};

TEST_F(ChunkChangeTrackerTest, receiveBlockUpdate1){
    ns_cct::MockPlayer* p = new ns_cct::MockPlayer();

    tracker->addPlayer(p);
    tracker->addChange(Coordinate<int>(2,4,3), Block());
    tracker->sendChanges();

    vector<JobTicket*> received = p->getMockConn()->received;
    int nPackets = received.size();


    ASSERT_TRUE(nPackets == 2)
        << "Player received " << nPackets << " packets instead of 2\n";

};

TEST_F(ChunkChangeTrackerTest, receiveBlockUpdate2){
    ns_cct::MockPlayer* p = new ns_cct::MockPlayer();

    tracker->addPlayer(p);
    tracker->sendChanges();
    tracker->addChange(Coordinate<int>(2,4,3), Block());
    tracker->sendChanges();

    vector<JobTicket*> received = p->getMockConn()->received;
    int nPackets = received.size();


    ASSERT_TRUE(nPackets == 2)
        << "Player received " << nPackets << " packets instead of 2\n";

};

TEST_F(ChunkChangeTrackerTest, twoPlayers){
    ns_cct::MockPlayer* p = new ns_cct::MockPlayer();
    ns_cct::MockPlayer* p2 = new ns_cct::MockPlayer();

    tracker->addPlayer(p);
    tracker->sendChanges();
    tracker->addChange(Coordinate<int>(2,4,3), Block());
    tracker->sendChanges();

    tracker->addPlayer(p2);
    tracker->sendChanges();

    vector<JobTicket*> received = p2->getMockConn()->received;
    int nPackets = received.size();


    ASSERT_TRUE(nPackets == 2)
        << "Player received " << nPackets << " packets instead of 2\n";

};
