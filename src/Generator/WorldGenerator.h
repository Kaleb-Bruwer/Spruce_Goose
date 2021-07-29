#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include <vector>
#include <thread>
#include <chrono>

#include "../Datastructures/BlockingQueueCounting.h"
#include "../JobTickets/JobTicket.h"
#include "TerrainInterface.h"

using namespace std;

class GeneratorThread;

class WorldGenerator : public TerrainInterface{
private:
    const unsigned int nStartingThreads = 64;

    BlockingQueueCounting<JobTicket*> inQueue;
    vector<GeneratorThread*> generators;

    //Responsible for creating & destroying generators to match demand
    thread* myThread = 0;
    const chrono::seconds sleepLen{2};
    bool stop = false;
    void run();

    // inline static WorldGenerator* instance = 0;
    // inline static mutex constructMutex;

    WorldGenerator();
    ~WorldGenerator();
    friend TerrainInterface;

public:
    // static WorldGenerator* getInstance();

    void pushJob(JobTicket* job);
    void quit();
};

#endif
