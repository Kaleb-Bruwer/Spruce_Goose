#include "TerrainPort.h"

using namespace std;

TerrainPort::TerrainPort(){
    myThread = new thread(&TerrainPort::run, this);
}

TerrainPort::~TerrainPort(){}

TerrainPort* TerrainPort::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new TerrainPort();
        }
        constructMutex.unlock();
    }
    return instance;
}

// Should run for entirety of program lifetime, so memory cleanup isn't really important
void TerrainPort::run(){
    while(!quit){
        // Read and handle incoming requests
        if(!handleJobTickets())
            this_thread::sleep_for(sleepLen);

        // Read incoming packets

    }
}

bool TerrainPort::handleJobTickets(){
    bool flag = true;
    bool foundJob = false;
    while(flag){
        // TODO: add a time limit to this loop to prevent missing keep alives
        JobTicket* job = inQueue.pop();
        if(!job)
            return foundJob;
        else
            foundJob = true;

        if(job->getType() == GENERATECHUNKREQUEST){
            // Get this chunk somehow
        }
        job->drop();
    }
}
