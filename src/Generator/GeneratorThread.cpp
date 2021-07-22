#include "GeneratorThread.h"

#include "../World/SynchedArea.h"

#include "../JobTickets/WorldToGenerator/GenerateChunkRequest.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"

using namespace std;

GeneratorThread::GeneratorThread(BlockingQueueCounting<JobTicket*>* q){
    queue = q;

    myThread = new thread(&GeneratorThread::run, this);
}

GeneratorThread::~GeneratorThread(){
    if(myThread){
        stop = true;
        myThread->join();
        delete myThread;
    }
}

void GeneratorThread::generateChunk(JobTicket* j){
    GenerateChunkRequest* job = (GenerateChunkRequest*)j;
    Chunk* chunk = generateChunk(job->chunkPos);
    chunk->activateChangeLog();
    ChunkFromGenerator* job2 = new ChunkFromGenerator();
    job2->chunk = chunk;
    job->origin->pushJob(job2);
}

void GeneratorThread::run(){
    while(true){
        if(stop)
            return;

        JobTicket* job = queue->pop();
        if(job == 0){
            this_thread::sleep_for(sleepLen);
            continue;
        }

        if(job->getType() == GENERATECHUNKREQUEST){
            generateChunk(job);
        }
        job->drop();
    }
}
