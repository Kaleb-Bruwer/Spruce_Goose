#include "CompChunkPacket.h"

#include <thread>
#include <cstring>

#include "../../JobTickets/SendPacket.h"
#include "../../Protocol/ChunkWriter.h"

using namespace std;

CompChunkPacket::CompChunkPacket(Chunk* c){
    thread t(&CompChunkPacket::run, this, c);
    t.detach();
}

CompChunkPacket::~CompChunkPacket(){
    job->drop();
}

JobTicket* CompChunkPacket::getJob(){
    if(done){
        return job;
    }
    return 0;
}

//This is run on a separate thread so it doesn't hold up program execution
void CompChunkPacket::run(Chunk* chunk){

    ChunkWriter writer;
    writer.writeChunk(chunk);

    SendPacket* j = new SendPacket();

    j->size = writer.getIndex();
    j->data = new char[j->size];
    char* buff = writer.getBuffer();
    memcpy(&(j->data[0]), buff, j->size);
    j->pickup();
    job = j;

    done = true;

}
