#ifndef GENERATORTHREAD_H
#define GENERATORTHREAD_H

#include <chrono>
#include <thread>

#include "../Datastructures/Coordinate/ChunkCoord.h"
#include "../Datastructures/BlockingQueueCounting.h"

#include "../JobTickets/JobTicket.h"

using namespace std;

class Chunk;

class GeneratorThread{
protected:
    //Only sleeps if it didn't have work, otherwise it immediately checks for
    //another job
    thread* myThread = 0;
    const chrono::milliseconds sleepLen{10};
    bool stop = false;

    BlockingQueueCounting<JobTicket*>* queue = 0;

    void generateChunk(JobTicket* job);

    virtual Chunk* generateChunk(ChunkCoord pos) = 0;
public:
    GeneratorThread(BlockingQueueCounting<JobTicket*>*);
    ~GeneratorThread();

    void run();
    void quit(){stop = true;};
};

#endif
