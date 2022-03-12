#include "ChunkWriter.h"

#include "../World/Chunk/Chunk.h"

using namespace std;

ChunkWriter::ChunkWriter(unsigned int size) : AdvancedWriter(size){};

void ChunkWriter::writeChunk(Chunk* chunk){
    writePacketID(0x21);

    ChunkCoord cCoord = chunk->getChunkCoord();

    baseThis << (int) cCoord.x;
    baseThis << (int) cCoord.z;

    baseThis << (bool) true; //Ground-up continuous

    unsigned short bitMask = chunk->getSectionBitMask();
    baseThis << bitMask;
    baseThis << (short) 0; //addBitMask

    ChunkWriter raw;
    int compressedSize = raw.writeData(chunk);

    baseThis << compressedSize;
    PacketWriter& rawRef = (PacketWriter&) raw;
    baseThis << rawRef;

    addMsgLen();
}

int ChunkWriter::writeData(Chunk* chunk){
    unsigned char buffer[32768];

    //Write data to raw
    //write Block type array
    for(int s=0; s<16; s++){
        if(chunk->writeBlockIDs(&(buffer[0]), s)){
            writeArr(&(buffer[0]), 4096);
        }
    }

    //write block metadata array
    for(int s=0; s<16; s++){
        if(chunk->writeMetaData(&(buffer[0]), s)){
            writeArr(&(buffer[0]), 2048);
        }
    }

    //write block light array
    for(int s=0; s<16; s++){
        if(chunk->writeLightVals(&(buffer[0]), s)){
            writeArr(&(buffer[0]), 2048);
        }
    }

    //sky light array
    unsigned char lightVal = 0x15 || (0x15 << 4);

    unsigned short length = 0;
    unsigned short bitMask = chunk->getSectionBitMask();
    for(int i=0; i<16; i++){
        if(bitMask % 2 == 1)
            length ++;
        bitMask = bitMask >> 1;
    }

    memset(&(buffer[0]), lightVal, length * 2048);
    writeArr(&(buffer[0]), length * 2048);

    //biomes
    memset(&(buffer[0]), 1, 256);
    writeArr(&(buffer[0]), 256);

    return compressAll();
}
