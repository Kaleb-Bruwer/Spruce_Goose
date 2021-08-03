#include "GenPlayer.h"

#include "../World/Chunk/Chunk.h"
#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"
#include "../World/SynchedArea.h"

using namespace std;

int GenPlayer::activate(string username){
    connection.openConnection((std::string)"127.0.0.1", 10000, username);
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

    activeCluster.add(coord, dest);
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
    outstanding -= newChunks.size();
    for(int i=0; i< newChunks.size(); i++){
        ChunkCoord coord = newChunks[i]->getChunkCoord();
        SynchedArea* dest = activeCluster.getDest(coord);
        if(dest){
            cout << "Sending " << coord << " to dest\n";
            sendChunk(newChunks[i], dest);
            activeCluster.remove(coord);
        }
        else{
            cout << "Adding " << coord << " to buffer\n";
            buffer.push_back(newChunks[i]);

        }
    }
    return buffer;
}
