#pragma once

#include <thread>

#include "../Datastructures/BlockingQueue.h"
#include "../JobTickets/JobTicket.h"

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

    TerrainPort();
    ~TerrainPort();

    bool handleJobTickets();

public:
    static TerrainPort* getInstance();
    void pushJob(JobTicket* job){
        job->pickup();
        inQueue.push(job);
    };
};
