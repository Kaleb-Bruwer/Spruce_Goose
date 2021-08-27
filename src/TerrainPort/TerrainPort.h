#pragma once

#include <thread>
#include <map>
#include <sys/epoll.h>

#include "../Generator/TerrainInterface.h"
#include "../Datastructures/BlockingQueue.h"
#include "../JobTickets/JobTicket.h"
#include "../JobTickets/WorldToGenerator/GenChunkReq2.h"
#include "Cluster.h"
#include "ClusterGroup.h"
#include "GenPlayer.h"

class Chunk;
class SynchedArea;
class JobTicket;

struct TimestampedChunk{
    chrono::high_resolution_clock::time_point time;
    Chunk* chunk;
};

// Received from TerrainInterface::getInstance
// TO switch terrain provider, see said function

class TerrainPort : public TerrainInterface{
private:
    int epfd;

    // This is an arbitrary value, just change it if needed
    static const int maxGenPlayers = 32;
    struct epoll_event events[maxGenPlayers];

    BlockingQueue<JobTicket*> inQueue;

    // Requests that are simply waiting upon GenPlayers
    vector<GenChunkReq2*> inRequests;

    // Multithreading-related
    thread* myThread = 0;
    bool quit = false;
    const chrono::milliseconds sleepLen{10};
    void run(); //main loop for this class

    void read();

    // Clusters
    int renderDistance = 4;
    int numGenPlayers = 0;
    GenPlayer players[maxGenPlayers];
    map<int,int> playersBySockets;

    // Buffer (timestamps are used to remove old, unused values)
    map<ChunkCoord, TimestampedChunk> buffer;

    TerrainPort();
    ~TerrainPort();

    bool handleJobTickets();
    void handleQueue();
    void addSockToEP(int sock);

    // Assumes that chunks will only be requested once
    void getChunk(ChunkCoord coord, SynchedArea* returnAddr);

    // getChunks adds to buffer if not handled, tryGetChunks simply returns false
    bool tryGetChunks(GenChunkReq2* job);

    void sendChunk(Chunk* c, SynchedArea* dest);

    bool findCompatiblePlayer(GenChunkReq2* job);
    bool findOpenPlayer(GenChunkReq2* job);


    friend class TerrainInterface;

public:
    void pushJob(JobTicket* job){
        job->pickup();
        inQueue.push(job);
    };
};
