#ifndef WORLD_H
#define WORLD_H

#include <thread>

#include "ThreadArea.h"
#include "../Datastructures/BlockingQueue.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/Coordinate/ChunkCoord.h"
#include "../Datastructures/ChunkMap/SynchedAreaMap.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"
#include "../JobTickets/JobTicket.h"

using namespace std;

class World{
private:
    vector<ThreadArea*> threadAreas;
    SynchedAreaMap synchedAreaMap;

    Coordinate<int> spawnpoint;
    thread* myThread = 0;
    bool quit = false;

    BlockingQueue<JobTicket*> inQueue;

    void run();
    void handleJobTickets();

    void addPlayer(JobTicket* job);
    void handleChat(JobTicket* job);
    void handleExpandRequest(JobTicket* job);
    void handleRedirect(JobTicket* job);

    ThreadArea* getThreadArea(ChunkCoord &c);
    ThreadArea* getThreadArea(Coordinate<double> &c);

    void joinThreadAreas();

public:
    World(bool multiT=true);
    ~World();
    void join();
    void loop(); //Single threaded mode

    void pushJob(JobTicket* job){
        job->pickup();
        inQueue.push(job);
    };

    //Used by SynchedArea to add ThreadArea to vector, that's all it does
    //(executed on World's thread, though)
    void addThreadArea(ThreadArea* t);
};

#endif
