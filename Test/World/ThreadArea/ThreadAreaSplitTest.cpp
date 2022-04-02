#include "ThreadAreaTestHelpers.h"


using namespace std;

/*
split() never actually accesses World or SynchedArea, it only copies the ptrs
So completely fake values that were typecast should work fine
*/

class ThreadAreaSplitTest : public ::testing::Test{
protected:
    World* world = (World*) 0x01;
    SynchedArea* synchedArea = (SynchedArea*) 0x02;
    SynchedArea* destSynchedArea = (SynchedArea*) 0x03;
    // false disables creation of its own thread
    ThreadArea tArea = ThreadArea(synchedArea, world, false);



    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

};

TEST_F(ThreadAreaSplitTest, emptySplit){
    //Split off a region with no chunks in it

    // All of these are in Region 0,0
    vector<ChunkCoord> positions = {
        ChunkCoord(0,0),
        ChunkCoord(0,1),
        ChunkCoord(1,0),
        ChunkCoord(1,1),
        ChunkCoord(1,2)
    };

    tAreaSetChunks(tArea, positions);

    vector<RegionCoord> splitCoords = {
        RegionCoord(1,1)
    };

    ThreadArea* newArea = tArea.split(splitCoords, destSynchedArea);

    ASSERT_TRUE(newArea == 0) << "split should have returned nothing";

    tAreaUnsetChunks(tArea, positions);
}

TEST_F(ThreadAreaSplitTest, realSplit){
    //Split off a region with no chunks in it

    // All of these are in Region 0,0
    vector<ChunkCoord> positions1 = {
        ChunkCoord(0,0),
        ChunkCoord(1,1),
        ChunkCoord(1,2)
    };

    // All of these are in Region 0,1
    vector<ChunkCoord> positions2 = {
        ChunkCoord(0,32),
        ChunkCoord(1,33),
        ChunkCoord(1,34)
    };

    tAreaSetChunks(tArea, positions1);
    tAreaSetChunks(tArea, positions2);

    vector<RegionCoord> splitCoords = {
        RegionCoord(0,1)
    };

    ThreadArea* newArea = tArea.split(splitCoords, destSynchedArea);

    ASSERT_TRUE(newArea != 0) << "Didn't create a new ThreadArea";

    // Remaining chunks intact
    verifyChunks(tArea, positions1);
    // transferred chunks placed over
    verifyChunksNull(tArea, positions2);

    // Chunks placed over
    verifyChunks(*newArea, positions2);

    tAreaUnsetChunks(tArea, positions1);
}

TEST_F(ThreadAreaSplitTest, entitySplit){
    //Split off a region with no chunks in it

    // All of these are in Region 0,0
    vector<ChunkCoord> positions1 = {
        ChunkCoord(0,0),
        ChunkCoord(1,1),
        ChunkCoord(1,2)
    };

    // All of these are in Region 0,1
    vector<ChunkCoord> positions2 = {
        ChunkCoord(0,32),
        ChunkCoord(1,33),
        ChunkCoord(1,34)
    };

    tAreaSetChunks(tArea, positions1);
    tAreaSetChunks(tArea, positions2);

    // Add entities
    vector<Entity*> ents1 = {
        new Item(Block(5), Coordinate<double>(4.2, 12, 15.3)),
        new Item(Block(2), Coordinate<double>(24.2, 2, 21.3)),
        new Item(Block(3), Coordinate<double>(4.2, 12.2, 42))
    };

    vector<Entity*> ents2 = {
        new Item(Block(5), Coordinate<double>(4.2, 12, 512.3)),
        new Item(Block(2), Coordinate<double>(2.2, 2, 519.3)),
        new Item(Block(3), Coordinate<double>(17.2, 12.2, 532))
    };

    int eids1[3];
    for(int i=0; i<3; i++){
        eids1[i] = ents1[i]->getEid();
        tArea.addEntity(ents1[i]);
    }
    int eids2[3];
    for(int i=0; i<3; i++){
        eids2[i] = ents2[i]->getEid();
        tArea.addEntity(ents2[i]);
    }

    vector<RegionCoord> splitCoords = {
        RegionCoord(0,1)
    };

    ThreadArea* newArea = tArea.split(splitCoords, destSynchedArea);

    // Verify entities
    for(int eid : eids1){
        Entity* ret = tArea.getEntity(eid);
        ASSERT_TRUE(ret != 0) << "Wrong entity removed";
        ret = newArea->getEntity(eid);
        ASSERT_TRUE(ret == 0) << "Wrong entity moved over";
    }


    for(int eid : eids2){
        Entity* ret = tArea.getEntity(eid);
        ASSERT_TRUE(ret == 0) << "Wrong entity remained";
        ret = newArea->getEntity(eid);
        ASSERT_TRUE(ret != 0) << "Entity not moved over";
    }

    tAreaUnsetChunks(tArea, positions1);
}
