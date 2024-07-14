#ifndef COMPCHUNKPACKET_H
#define COMPCHUNKPACKET_H

#include "ChunkChangeToken.h"
#include "Chunk.h"
#include "../../JobTickets/JobTicket.h"
#include "../../Protocol/PacketWriterOld.h"

using namespace std;

namespace ns_cct{
    class MockCompChunkPacket;
}

class CompChunkPacket : public ChunkChangeToken{
private:
    bool done = false;

    //Once done=true, this will contain a complete SendChunk packet
    JobTicket* job = 0;

    //These are only used by the compression thread
    void run(Chunk* c);
//    void writeData(Chunk* chunk, PacketWriterOld &p);

    CompChunkPacket(){};

    friend class ns_cct::MockCompChunkPacket;

public:
    CompChunkPacket(Chunk* c);
    ~CompChunkPacket(); //Don't delete until done=true

    bool isDone(){return done;};
    ChangeTokenType getType(){return COMPCHUNK;};

    JobTicket* getJob();
};

#endif
