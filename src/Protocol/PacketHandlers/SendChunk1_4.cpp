#include "SendChunk1_4.h"

#include "../../World/Chunk/Chunk.h"
#include "../PlayerConnection1_4_5.h"

using namespace std;

SendChunk1_4::SendChunk1_4(PlayerConnection1_4_5* p, vector<Chunk*> c){
    player = p;
    chunks = c;
    myThread = new thread(&SendChunk1_4::run, this);
}

SendChunk1_4::SendChunk1_4(PlayerConnection1_4_5* p, Chunk** c, int nChunks){
    player = p;

    chunks.resize(nChunks);
    for(int i=0; i<nChunks; i++){
        chunks[i] = c[i];
    }

    myThread = new thread(&SendChunk1_4::run, this);
}

SendChunk1_4::~SendChunk1_4(){
    myThread->join();
    delete myThread;
    for(Chunk* c : chunks)
        delete c;
}

void SendChunk1_4::writeData(Chunk* chunk, PacketWriterOld &p){
    PacketWriterOld raw; //Will be compressed and written over later

    unsigned char buffer[4096];

    //Write data to raw
    //write Block type array
    for(int s=0; s<16; s++){
        if(chunk->writeBlockIDs(&(buffer[0]), s)){
            raw.writeArr(&(buffer[0]), 4096);
        }
    }

    //write block metadata array
    for(int s=0; s<16; s++){
        if(chunk->writeMetaData(&(buffer[0]), s)){
            raw.writeArr(&(buffer[0]), 2048);
        }
    }

    //write block light array
    for(int s=0; s<16; s++){
        if(chunk->writeLightVals(&(buffer[0]), s)){
            raw.writeArr(&(buffer[0]), 2048);
        }
    }

    for(int i=0; i<256; i++){
        raw << (char)1; //Plains biome
    }

    //compress raw
    int compressedSize = raw.compressAll();

    //write raw to p
    p << compressedSize;
    p << raw;
}

void SendChunk1_4::run(){
    player->sendLock.lock();
    for(Chunk* chunk : chunks){
        PacketWriterOld p(2048);
        p.writePacketID(0x33);

        ChunkCoord cCoord = chunk->getChunkCoord();

        p << (int) cCoord.x;
        p << (int) cCoord.z;
        p << (bool) true; //Ground-up continuous

        //bitmask
        unsigned short bitMask = chunk->getSectionBitMask();
        p << bitMask; //Primary bitmap
        p << (short) 0; //Add bitmap
        writeData(chunk, p);
        player->sendMessage(p);
    }
    done = true;
    player->sendLock.unlock();
}
