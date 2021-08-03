#include "TerrainPort.h"

#include "../JobTickets/WorldToGenerator/GenerateChunkRequest.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"
#include "../World/SynchedArea.h"


using namespace std;

TerrainPort::TerrainPort(){
    Cluster::renderDistance = 4;

    epfd = epoll_create(maxGenPlayers);

    myThread = new thread(&TerrainPort::run, this);
}

TerrainPort::~TerrainPort(){}

void TerrainPort::addSockToEP(int sock){
    cout << "Added GenPlayer " << sock << endl;
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    ev.data.fd = sock;
    int res = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);

    if(res < 0){
        cout << "epoll_ctl ERROR in TerrainPort::addSockToEP\n";
        cout << "Response " << res << " when adding sock = " << sock << endl;
    }
}

// Should run for entirety of program lifetime, so memory cleanup isn't really important
void TerrainPort::run(){
    while(!quit){
        // Read and handle incoming requests
        if(!handleJobTickets())
            // Will sleep if no jobs were found
            this_thread::sleep_for(sleepLen);

        // Read & handle incoming packets
        read();
        processClusters();
    }
}

void TerrainPort::read(){
    //timeout is in ms
    const int timeout = 50;
    int numToRead = epoll_wait(epfd, &(events[0]), maxGenPlayers, timeout);

    for(int i=0; i<numToRead; i++){
        int sock = events[i].data.fd;
        int playerIndex = playersBySockets[sock];

        // Chunks that must be added to buffer
        vector<Chunk*> b = players[playerIndex].readMessage();

        for(Chunk* chunk : b){
            struct TimestampedChunk chunkT;
            chunkT.chunk = chunk;
            chunkT.time = chrono::high_resolution_clock::now();
            ChunkCoord coord = chunk->getChunkCoord();

            if(buffer.find(coord) != buffer.end()){
                Chunk* oldChunk = buffer[coord].chunk;
                if(oldChunk)
                    delete oldChunk;
            }
            buffer[coord] = chunkT;
        }
    }
}

void TerrainPort::processClusters(){
    int pIndex = 0;

    for(auto it = sendingQueue.begin(); it != sendingQueue.end();){
        if(!trySendCluster(*it, pIndex))
            return;
        it = sendingQueue.erase(it);
    }

    vector<Cluster> mustSend = clusters.getReadyClusters();
    // Function exits once mustSend is empty

    bool flag = true;
    int index = 0;
    while(flag){
        if(index >= mustSend.size())
            return;
        if(!trySendCluster(mustSend[index], pIndex)){
            flag = false;
        }
        else{
            index++;
        }
    }

    for(; index < mustSend.size(); index++){
        sendingQueue.push_back(mustSend[index]);
    }
}

bool TerrainPort::trySendCluster(Cluster a, int &i){
    for(; i<numGenPlayers; i++){
        if(players[i].onStandby()){
            players[i].setCluster(a);
            return true;
        }
    }

    // If not, create a new GenPlayer if possible
    if(numGenPlayers < maxGenPlayers){
        string username = "bot" + to_string(numGenPlayers);
        int sock = players[numGenPlayers].activate(username);
        addSockToEP(sock);

        players[numGenPlayers].setCluster(a);
        numGenPlayers++;
        return true;
    }
    return false;
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
            GenerateChunkRequest* jobCast = (GenerateChunkRequest*) job;
            getChunk(jobCast->chunkPos, jobCast->origin);
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

    //TODO: If already in a request, just set returnAddr
    for(int i=0; i<numGenPlayers; i++){
        if(players[i].addChunk(coord, returnAddr)){
            return;
        }
    }


    // else, add to cluster
    clusters.addChunk(coord, returnAddr);
}

void TerrainPort::sendChunk(Chunk* c, SynchedArea* dest){
    cout << "Sending " << c->getChunkCoord() << " to dest\n";
    ChunkFromGenerator* job = new ChunkFromGenerator();
    job->chunk = c;
    dest->pushJob(job);
}
