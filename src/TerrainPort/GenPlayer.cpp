#include "GenPlayer.h"

#include "../World/Chunk/Chunk.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"
#include "../World/SynchedArea.h"


void GenPlayer::setCluster(Cluster c){
    activeCluster  = c;

    // Must check that cluster isn't already in buffer, since server won't
    // resend such chunks
    for(auto it = c.values.begin(); it != c.values.end();){
        Chunk* chunk = uncollected.getVal(it->coord);
        if(chunk){
            sendChunk(chunk, it->dest);
            it = c.values.erase(it);
        }
        else
            it++;
    }

    // Calculate new outstanding values
    outstanding = c.values.size();

    // Apply to connection


}

bool GenPlayer::addChunk(ChunkCoord coord, SynchedArea* dest){
    if(!activeCluster.fitsHere(coord))
        return false;

    Chunk* chunk = uncollected.getVal(coord);
    if(chunk){
        sendChunk(chunk, dest);
        return true;
    }

    ClusterVal val;
    val.coord = coord;
    val.dest = dest;
    activeCluster.values.push_back(val);
    return true;
}


void GenPlayer::sendChunk(Chunk* c, SynchedArea* dest){
    ChunkFromGenerator* job = new ChunkFromGenerator();
    job->chunk = c;
    dest->pushJob(job);
}

void GenPlayer::readMessage(){
    vector<Chunk*> newChunks = connection.readMessage();
    for(int i=0; i< newChunks.size(); i++){
        uncollected.setVal(newChunks[i]->getChunkCoord(), newChunks[i]);
    }
}
