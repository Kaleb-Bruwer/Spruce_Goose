#include "TerrainPort.h"

#include "../JobTickets/WorldToGenerator/GenerateChunkRequest.h"
#include "../JobTickets/WorldToGenerator/GenChunkReq2.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"
#include "../World/SynchedArea.h"


using namespace std;

TerrainPort::TerrainPort(){
    Cluster::renderDistance = 4;
    GenPlayer::renderDistance = 4;

    epfd = epoll_create(maxGenPlayers);

    myThread = new thread(&TerrainPort::run, this);
}

TerrainPort::~TerrainPort(){}

void TerrainPort::addSockToEP(int sock){
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
        handleQueue();
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
            ChunkCoord coord = chunk->getChunkCoord();
            if(buffer.find(coord) != buffer.end()){
                delete chunk;
            }
            else{
                struct TimestampedChunk chunkT;
                chunkT.chunk = chunk;
                chunkT.time = chrono::high_resolution_clock::now();

                buffer[coord] = chunkT;
            }
        }
    }
}

void TerrainPort::handleQueue(){
    for(int i=0; i<inRequests.size();){
        if(tryGetChunks(inRequests[i]))
            inRequests.erase(inRequests.begin() + i);
        else
            return;
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

        JobTicketType jobType = job->getType();

        switch(jobType){
        case GENERATECHUNKREQUEST:
            cout << "GENERATECHUNKREQUEST no longer supported in TerrainPort\n";
            break;

        case GENCHUNKREQ2:
            job->pickup();
            inRequests.push_back((GenChunkReq2*)job);
            break;
        }

        job->drop();
    }
}

bool TerrainPort::tryGetChunks(GenChunkReq2* job){
    // Check buffer for any needed chunks
    for(int i=0; i < job->chunks.size();){
        auto it = buffer.find(job->chunks[i]);

        if(it != buffer.end()){
            Chunk* c = it->second.chunk;
            sendChunk(c, job->origin);
            job->chunks.erase(job->chunks.begin() + i);
        }
        else{
            i++;
        }
    }
    if(job->chunks.size() == 0){
        job->drop();
        return true;
    }

    if(!findCompatiblePlayer(job))
        if(!findOpenPlayer(job)){
            return false;
        }
    return true;
}

bool TerrainPort::findCompatiblePlayer(GenChunkReq2* job){
    Coordinate<int> pos = job->playerPos;
    for(int i=0; i<numGenPlayers; i++){
        // Check for busy players that overlap
        if(!players[i].onStandby() && players[i].canFitNewCenter(pos)){
            players[i].shiftJob(job);
            cout << "Found compatible player\n";
            return true;
        }
    }
    return false;
}

bool TerrainPort::findOpenPlayer(GenChunkReq2* job){
    for(int i=0; i<numGenPlayers; i++){
        if(players[i].onStandby()){
            players[i].setJob(job);
            cout << "Found standby player\n";
            return true;
        }
    }

    if(numGenPlayers < maxGenPlayers){
        string name = "bot" + to_string(numGenPlayers);

        int sock = players[numGenPlayers].activate(name);
        addSockToEP(sock);
        playersBySockets[sock] = numGenPlayers;

        players[numGenPlayers].setJob(job);

        numGenPlayers++;
        cout << "Added new player\n";
        return true;
    }
    return false;
}


void TerrainPort::sendChunk(Chunk* c, SynchedArea* dest){
    ChunkFromGenerator* job = new ChunkFromGenerator();
    job->chunk = c;
    dest->pushJob(job);
}
