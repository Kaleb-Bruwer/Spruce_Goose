#include "GenPlayer.h"

#include <cmath>

#include "../World/Chunk/Chunk.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"
#include "../World/SynchedArea.h"

using namespace std;

int GenPlayer::activate(string username){
    connection.openConnection((std::string)"127.0.0.1", 10000, username);
    return connection.sock;
}

bool GenPlayer::canFitNewCenter(Coordinate<int> pos){
    // This might need optimization, depending on how often it runs
    ChunkCoord cCoord;
    cCoord.x = pos.x/16;
    cCoord.z = pos.z/16;

    for(int i=0; i<activeJob->chunks.size(); i++){
        int dist = max(abs(cCoord.x - activeJob->chunks[i].x), abs(cCoord.z - activeJob->chunks[i].z));
        if(dist > renderDistance)
            return false;
    }
    return true;
}


GenChunkReq2* GenPlayer::setJob(GenChunkReq2* job){
    GenChunkReq2* result = 0;
    if(activeJob){
        if(activeJob->chunks.size() > 0)
            result = activeJob;
        else{
            cout << "Drop " << activeJob << " in setJob\n";
            activeJob->drop();
        }
    }
    activeJob = job;
    activeJob->pickup();

    jobAge = chrono::high_resolution_clock::now();
    // Teleport
    connection.sendTeleport(activeJob->playerPos);

    return result;
}

void GenPlayer::shiftJob(GenChunkReq2* job){
    // Move remaining chunks from old job to new one
    job->chunks.insert(job->chunks.end(), activeJob->chunks.begin(), activeJob->chunks.end());
    cout << "Drop " << activeJob << " in shiftJob\n";
    activeJob->drop();
    activeJob = job;
    activeJob->pickup();

    jobAge = chrono::high_resolution_clock::now();
    connection.sendTeleport(activeJob->playerPos);
}

void GenPlayer::sendChunk(Chunk* c, SynchedArea* dest){
    ChunkFromGenerator* job = new ChunkFromGenerator();
    job->chunk = c;
    dest->pushJob(job);
}

vector<Chunk*> GenPlayer::readMessage(){
    vector<Chunk*> buffer;

    // Note that none of the chunks are deleted here, they're all passed on
    vector<Chunk*> newChunks = connection.readMessage();

    for(int i=0; i< newChunks.size(); i++){
        ChunkCoord coord = newChunks[i]->getChunkCoord();

        cout << "GenPlayer received chunk " << coord << endl;

        if(activeJob->contains(coord)){
            sendChunk(newChunks[i], activeJob->origin);
            activeJob->removeChunk(coord);
        }
        else{
            buffer.push_back(newChunks[i]);
        }
    }
    return buffer;
}
