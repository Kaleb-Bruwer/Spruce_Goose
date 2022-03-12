#include "WorldGenerator.h"

#include "SuperflatGenerator.h"
#include "TerrainGen1.h"

using namespace std;

WorldGenerator::WorldGenerator(){
    for(int i=0; i < nStartingThreads; i++){
        //generators.push_back(new SuperflatGenerator(&inQueue));
        generators.push_back(new TerrainGen1(&inQueue, 12345));
    }
    myThread = new thread(&WorldGenerator::run, this);
}

WorldGenerator::~WorldGenerator(){
    //Delete GeneratorThreads
    quit();
    //myThread will take longer to join, but can cause problems
    //if generators are deleted from right under it
    myThread->join();
    delete myThread;

    for(GeneratorThread* g : generators){
        delete g;
    }
}

void WorldGenerator::pushJob(JobTicket* job){
    job->pickup();
    inQueue.push(job);
}

void WorldGenerator::run(){
    while(true){
        if(stop)
            return;
        this_thread::sleep_for(sleepLen);
    }
}

void WorldGenerator::quit(){
    stop = true;
    for(GeneratorThread* g : generators){
        g->quit();
    }
}
