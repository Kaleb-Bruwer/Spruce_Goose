#ifndef SYNCHEDAREA_H
#define SYNCHEDAREA_H

#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "Region.h"

#include "../Datastructures/BlockingQueue.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"
#include "../Datastructures/ChunkMap/RegionMap.h"

#include "../JobTickets/JobTicket.h"

using namespace std;

//Used to keep group of ThreadAreas synchronized
//Also loads and holds regions
//Has count for how many ThreadAreas it has
//Owned by World

class World;
class ThreadArea;
class SynchedAreaMap;
class TerrainInterface;

class SynchedArea{
private:
    World* world;

    mutex m;

    //These vectors used when ThreadAreas are split, since
    //it needs to be doen on the TArea's thread
    vector<ThreadArea*> areasToSplit;
    vector<ThreadArea*> newAreas;
    condition_variable condSplit;
    bool splitContinue = false;
    vector<RegionCoord>* rC = 0;
    SynchedArea* newS = 0;


    //TODO: change available to vector of regionCoords
    RegionMap<bool> available; //Regions this SynchedArea owns
    RegionMap<Region*> regions; //Regions loaded from files
    RegionMap<int> claimedInRegion; //Number of claimed chunks in region
    ChunkMap<ThreadArea*> owned; //Which ThreadAreas own which chunks

    const chrono::milliseconds tickLen{50};
    chrono::high_resolution_clock::time_point nextTick;

    //!!!!!!!!!!!
    //This stuff is all related to myThread
    TerrainInterface* terrainInterface = 0;
    thread* myThread = 0;
    BlockingQueue<JobTicket*> inQueue;
    bool stop = false;
    const chrono::milliseconds sleepLen{10};

    void run();
    void handleJobTickets();

    void handleRedirect(JobTicket* job);
    void loadChunk(JobTicket* job); //Takes LoadChunk packet
    void loadChunk2(JobTicket* job); //takes GenChunkReq2 packet (newer version)
    void receiveChunk(JobTicket* job); //From Generator
    void receiveChunksTArea(JobTicket* job);
    void unclaimRegion(JobTicket* job);
    //!!!!!!!!!! (end of myThread stuff)

    int numMembers = 0;
    int nextNumMembers = 0;

    //These are used by sync function
    int done = 0;
    int out = 0;
    condition_variable conditionDone;
    condition_variable conditionOut;
    condition_variable conditionJoin;

    void maintenance(); //Called once at end of every tick

    //
    chrono::time_point<chrono::steady_clock> lastWrite;
    bool mustWrite(); //Used to decide when to write
    bool writePending = false;
    int neededAreas = 0;

    //Used for merging
    SynchedArea* mergeTarget = 0;
    bool canDelete = true; //set to false if a merge is incoming
    bool finished = false; //Set to true if all ThreadAreas have left after a merge

    //Used to set canDelete to true recursively for merge trains
    //WILL cause deadlock in case of circular reference
    void setCanDelete();
    SynchedArea* split(vector<RegionCoord>);

    //Exclusively used by split
    //purpose is to set up new SynchedArea before it starts
    void addThreadArea(ThreadArea* t);

    friend class SynchedAreaMap;

public:
    SynchedArea(World* w, bool makeT = true);

    //SynchedArea doesn't actually own the TreadAreas, so this doesn't delete them
    ~SynchedArea();

    //Used by ThreadArea, blocks until next tick to guaruntee synchronization
    void addThreadArea();

    //Call this before sync, otherwise you will break stuff
    // (you still need to call sync, but you won't be in the next tick)
    void decMembers(){nextNumMembers--;};

    //Used by ThreadArea
    //return value is ThreadArea that owns Chunk, same
    //as claimer if claim was successful. 0 if chunk is out of bounds
    //for this SynchedArea
    void releaseChunk(ChunkCoord c, ThreadArea* owner);
    virtual bool hasRegion(RegionCoord r);
    ThreadArea* claimChunk(ChunkCoord coord, ThreadArea* claimer);
    ThreadArea* getOwner(ChunkCoord coord);

    ThreadArea* getThreadArea(Coordinate<double> coord);
    ThreadArea* getThreadArea(ChunkCoord coord);

    //Used by ThreadArea
    SynchedArea* sync(ThreadArea* ); //Called by each ThreadArea

    void pushJob(JobTicket* job);

    bool getWritePending(){return writePending;}; //Used to notify ThreadAreas
    //used to stop myThread
    void quit(){stop = true;};
};

#endif
