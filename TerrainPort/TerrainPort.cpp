#include "TerrainPort.h"

#include "../JobTickets/WorldToGenerator/GenerateChunkRequest.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"

using namespace std;

TerrainPort::TerrainPort(){
    clusters.setRenderDistance(4);
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

void TerrainPort::getChunk(ChunkCoord coord, SynchedArea* returnAddr){
    // Check buffer
    auto it = buffer.find(coord);
    if(it != buffer.end()){
        // Is in the buffer
        sendChunk(it->second.chunk, returnAddr);
        buffer.erase(it);
        return;
    }

}

void TerrainPort::sendChunk(Chunk* c, SynchedArea* dest){
    ChunkFromGenerator* job = new ChunkFromGenerator();
    job->chunk = c;
    dest->pushJob(job);
}
