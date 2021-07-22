#include "PacFullChunk1_16_3.h"

#include "../PlayerConnection.h"

using namespace std;

void PacFullChunk1_16_3::writeFullChunk(Chunk* chunk, PacketWriter &p){
    cout << "PacFullChunk1_16_3::writeFullChunk commented out\n";
    /*
    //Chunk coords (int, int)
    ChunkCoord cCoord = chunk->getChunkCoord();

    p << cCoord.x;
    p << cCoord.z;

    //Full Chunk (bool)
    p << (bool)true;

    //Primary Bit Mask (Varint)
    //Indicates which chunkSections are included
    Varint bitMask(getSectionBitMask(chunk));
    p << bitMask;

    //Heightmaps (NBT)
    Tag_Compound* heightMap = createHeightMap(chunk);
    p << *heightMap;
    delete heightMap;

    //Biomes
    Varint biomeArrLen(0);
    p << biomeArrLen;
    /*
        for(int i=0; i<1024; i++){
        p << (int) 127; //Void biome
    }
    *//*

    //Write sections to separate PacketWriter pSections
    //Get pSections's length & write to main PacketWriter
    //Merge PacketWriters

    PacketWriter pSections;
    for(int i=0; i<chunk->numSections; i++){
        if(!chunk->sections[i])
            continue;
        writeChunkSection(chunk->sections[i],pSections);
    }
    Varint pSectionsSize(pSections.getIndex());
    p << pSectionsSize;

    p << pSections;

    Varint numBlockEntities(0);
    p << numBlockEntities;
    */
}

void PacFullChunk1_16_3::handle(Chunk* chunk){
    PacketWriter p(2048);
    Varint packetID(0x20);
    p << packetID;

    writeFullChunk(chunk, p);
    p.addMsgLen();
    sendMessage(p);
}
