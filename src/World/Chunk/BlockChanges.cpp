#include "BlockChanges.h"

#include "../../JobTickets/SendPacket.h"
#include "../../Protocol/BlockChangeWriter.h"

using namespace std;

void BlockChanges::addChange(Coordinate<int> p, Block b){
    if(job){
        job->drop();
        job = 0;
    }

    pos.push_back(p);
    blocks.push_back(b);
}


void BlockChanges::merge(BlockChanges* rhs){
    if(job){
        job->drop();
        job = 0;
    }

    int oldSize = pos.size();
    int newSize = oldSize + rhs->pos.size();

    pos.resize(newSize);
    blocks.resize(newSize);

    int j = 0;
    for(int i=oldSize; i<newSize; i++){
        pos[i] = rhs->pos[j];
        blocks[i] = rhs->blocks[j];
        j++;
    }
}

JobTicket* BlockChanges::getJob(){
    if(job)
        return job;

    BlockChangeWriter writer;
    writer.writeBlockChanges(cCoord, pos, blocks);
    
    SendPacket* j = new SendPacket();

    j->size = writer.getIndex();
    j->data = new char[j->size];
    char* buffer = writer.getBuffer();
    memcpy(j->data, buffer, j->size);
    job = j;

    job->pickup();
    return job;
}
