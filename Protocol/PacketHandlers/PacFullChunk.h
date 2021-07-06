#ifndef PACFULLCHUNK_H
#define PACFULLCHUNK_H

#include "../../World/Chunk/Chunk.h"
#include "../../World/Chunk/ChunkSection.h"
#include "../../Datastructures/NBT/Tag_Compound.h"

using namespace std;

class PlayerConnection;

class PacFullChunk{
protected:
    PlayerConnection* player;

    void sendMessage(PacketWriter &p);

    //Chunk
    Tag_Compound* createHeightMap(Chunk* c);
    virtual void writeFullChunk(Chunk* c, PacketWriter &pw) = 0;

    //ChunkSection
    //PacFullChunk1_4_5 has a version of writeChunkSection() that takes
    //slightly different parameters
    virtual void writeChunkSection(ChunkSection* s, PacketWriter &pw);
    int bitsPerBlock(ChunkSection* s); //Helper function
    void writePalette(ChunkSection* s, PacketWriter &pw);

public:
    PacFullChunk(PlayerConnection* p){
        player = p;
    }

    //main function
    virtual void handle(Chunk* c) = 0;
};

#endif
