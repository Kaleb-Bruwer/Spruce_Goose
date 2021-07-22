#include "PacFullChunk.h"

#include <cmath>
#include <iostream>

#include "../PlayerConnection.h"

using namespace std;

void PacFullChunk::sendMessage(PacketWriter &p){
        player->sendMessage(p);
}

Tag_Compound* PacFullChunk::createHeightMap(Chunk* c){
    cout << "PacFullChunk::createHeightMap commented out\n";
    return 0;

    /*
    Tag_Compound* result = new Tag_Compound(); //Doesn't have a name
    result->setName("");
    Tag_Long_Array* tagArr = new Tag_Long_Array();
    tagArr->setName("MOTION_BLOCKING");
    result->addItem(tagArr);
    long long* array = new long long[37];
    tagArr->setArray(array, 37);

    unsigned short* heightMap = new unsigned short[256];
    for(int i=15; i>=0; i--){
        if(c->sections[i]) //Breaks if heightMap is completely filled in
            if(c->sections[i]->fillHeightMap(heightMap, i))
                break;
    }

    int indexS = 0;
    for(int indexA=0; indexA < 37; indexA++){
        int offset = 0;
        array[indexA] = 0;
        for(int i=0; i<7; i++){
            if(indexS >=256)
                break;
            array[indexA] |= ((long long)heightMap[indexS] << offset);
            offset += 9;
            indexS++;
        }
    }
    delete [] heightMap;

    return result;
    */
}

void PacFullChunk::writeChunkSection(ChunkSection* s, PacketWriter &p){
    cout << "PacFullChunk::writeChunkSection commented out\n";

    /*
    //blockCount (u short)
    p << s->nonAirCount;

    //Bits per block
    char bitsPerB = bitsPerBlock(s);
    p << bitsPerB;

    //Palette
    writePalette(s, p);

    //Length of data array
    int numLongs = (bitsPerB * s->sectionSize)/64;
    Varint arrLen(numLongs);
    p << arrLen;

    //Create compacted array
    long long* blocksCompact = new long long[numLongs];
    int indexB = 0;
    int offset = 0;

    blocksCompact[0] = 0; //Rest are initialized in loop

    for(int i=0; i<s->sectionSize; i++){
        blocksCompact[indexB] |= ((long long)s->blocks[i].id << offset);
        if(offset > 64-bitsPerB){
            indexB++;
            offset = offset -64 + bitsPerB;
            blocksCompact[indexB] = ((long long)s->blocks[i].id >> (bitsPerB - offset));
        }
        else if(offset == 64-bitsPerB){
            indexB++;
            offset = 0;
            if(indexB < arrLen.getInt())
                blocksCompact[indexB] = 0;
        }
        else{
            offset += bitsPerB;
        }
    }
    for(int i=0; i<numLongs; i++){
        p << blocksCompact[i];
    }
    delete [] blocksCompact;
    */
}

int PacFullChunk::bitsPerBlock(ChunkSection* s){
    /*
    int size = s->palette.size();
    if(size == 0)
        return 4;
    return max(4, (int) ceil(log2(size)));
    */
    return 4;
}

void PacFullChunk::writePalette(ChunkSection* s, PacketWriter &p){
    /*
    Varint paletteSize(s->palette.size());
    p << paletteSize;

    for(int i=0; i<paletteSize.getInt(); i++){
        Varint temp(s->palette[i]);
        p << temp;
    }
    */
}
