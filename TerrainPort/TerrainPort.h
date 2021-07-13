#pragma once

#include <thread>

#include "../Datastructures/BlockingQueue.h"
#include "../JobTickets/JobTicket.h"
#include "Cluster.h"
#include "ClusterGroup.h"

class Chunk;
class SynchedArea;

struct TimestampedChunk{
    chrono::high_resolution_clock::time_point time;
    Chunk* chunk;
};

class TerrainPort{
private:
    BlockingQueue<JobTicket*> inQueue;

    // Multithreading-related
    thread* myThread = 0;
    bool quit = false;
    const chrono::milliseconds sleepLen{10};
    void run(); //main loop for this class

    // Singleton-related
    inline static TerrainPort* instance = 0;
    inline static mutex constructMutex;

    // Clusters
    int renderDistance = 4;
    ClusterGroup clusters;

    // Buffer (timestamps are used to remove old, unused values)
    map<ChunkCoord, TimestampedChunk> buffer;

    TerrainPort();
    ~TerrainPort();

    bool handleJobTickets();

    // Assumes that chunks will only be requested once
    void getChunk(ChunkCoord coord, SynchedArea* returnAddr);
    void sendChunk(Chunk* c, SynchedArea* dest);

public:
    static TerrainPort* getInstance();
    void pushJob(JobTicket* job){
        job->pickup();
        inQueue.push(job);
    };
};
