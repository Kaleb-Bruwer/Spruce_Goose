#include "GenPlayer.h"

#include "../World/Chunk/Chunk.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"
#include "../World/SynchedArea.h"

int GenPlayer::activate(){
    connection.openConnection((std::string)"127.0.0.1", 10000);
    return connection.sock;
}

void GenPlayer::setCluster(Cluster c){
    activeCluster  = c;

    // Calculate new outstanding values
    outstanding = c.values.size();

    // Apply to connection
    connection.sendTeleport(c.center);

}

bool GenPlayer::addChunk(ChunkCoord coord, SynchedArea* dest){
    if(!activeCluster.fitsHere(coord))
        return false;

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

vector<Chunk*> GenPlayer::readMessage(){
    vector<Chunk*> buffer;

    // Note that none of the chunks are deleted here, they're all passed on
    vector<Chunk*> newChunks = connection.readMessage();
    for(int i=0; i< newChunks.size(); i++){
        ChunkCoord coord = newChunks[i]->getChunkCoord();
        SynchedArea* dest = activeCluster.getDest(coord);
        if(dest){
            sendChunk(newChunks[i], dest);
            activeCluster.remove(coord);
        }
        else{
            buffer.push_back(newChunks[i]);

        }
    }
    return buffer;
}
