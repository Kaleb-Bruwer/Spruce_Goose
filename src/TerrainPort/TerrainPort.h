#pragma once

#include <thread>
#include <map>
#include <sys/epoll.h>

#include "../Datastructures/BlockingQueue.h"
#include "../JobTickets/JobTicket.h"
#include "Cluster.h"
#include "ClusterGroup.h"
#include "GenPlayer.h"

class Chunk;
class SynchedArea;

struct TimestampedChunk{
    chrono::high_resolution_clock::time_point time;
    Chunk* chunk;
};

class TerrainPort{
private:
    int epfd;

    // This is an arbitrary value, just change it if needed
    static const int maxGenPlayers = 32;
    struct epoll_event events[maxGenPlayers];

    BlockingQueue<JobTicket*> inQueue;

    // Multithreading-related
    thread* myThread = 0;
    bool quit = false;
    const chrono::milliseconds sleepLen{10};
    void run(); //main loop for this class

    void read();
    void processClusters();

    // Singleton-related
    inline static TerrainPort* instance = 0;
    inline static mutex constructMutex;

    // Clusters
    int renderDistance = 4;
    ClusterGroup clusters;
    GenPlayer players[maxGenPlayers];
    map<int,int> playersBySockets;

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
