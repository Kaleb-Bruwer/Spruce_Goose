#include "CompressedChunk.h"

#include <cstring>
#include <cmath>

#include "../../Datastructures/NBT/Tag_Compound.h"
#include "../../Datastructures/NBT/Tag_Byte.h"
#include "../../Datastructures/NBT/Tag_Int.h"
#include "../../Datastructures/NBT/Tag_Byte_Array.h"
#include "../../Datastructures/NBT/Tag_List.h"

#include "../../Datastructures/Coordinate/RegionCoord.h"
#include "Chunk.h"
#include "../../zlib/zlib.h"
#include "../../Protocol/ProtocolHelpers.h" //switchEndian()

using namespace std;

CompressedChunk::CompressedChunk(char* raw, ChunkCoord c){
    coord = c;
    int l;
    memcpy((void*) &l, raw, 4);
    switchEndian((void*) &l, 4);
    len = l;

    compressionScheme = raw[4];
    if(compressionScheme != 2){
        cout << "Unfamiliar compression scheme on chunk " << (int)compressionScheme << endl;
    }
    data = new char[len];
    memcpy(data, &(raw[5]), len);
}

CompressedChunk::CompressedChunk(Chunk* c){
    unsigned char buffer[65536];

    Tag_Compound* base = new Tag_Compound();

    Tag_Compound* level = new Tag_Compound();
    level->setName("Level");
    base->addItem(level);

    Tag_List* sections = new Tag_List(10, 16);//Consists of 16 Tag_Compounds
    sections->setName("Sections");
    level->addItem(sections);

    c->writeBlockIDs(&(buffer[0]));
    int bufferIndex = 0;

    //Add region coord
    coord = c->getChunkCoord();
    RegionCoord rCoord = coord.getRegion();

    Tag_Int* xPos = new Tag_Int(rCoord.x);
    xPos->setName("xPos");
    level->addItem(xPos);

    Tag_Int* zPos = new Tag_Int(rCoord.z);
    zPos->setName("zPos");
    level->addItem(zPos);

    //Makes it easier to repeatedly loop it
    Tag_Compound* sectDirect[16];

    for(int i=0; i<16; i++){
        Tag_Compound* sect = new Tag_Compound();
        sectDirect[i] = sect;
        sections->addItem(sect);

        Tag_Byte* y = new Tag_Byte(i);
        y->setName("Y");
        sect->addItem(y);

        Tag_Byte_Array* blocks = new Tag_Byte_Array();
        blocks->setName("Blocks");
        char* arr = new char[4096];
        memcpy(arr, &(buffer[bufferIndex]), 4096);
        blocks->setArray(arr, 4096);
        sect->addItem(blocks);
        bufferIndex += 4096;
    }

    c->writeMetaData(&(buffer[0]));
    bufferIndex = 0;

    for(int i=0; i<16; i++){
        Tag_Byte_Array* meta = new Tag_Byte_Array();
        meta->setName("Data");
        char* arr = new char[2048];
        memcpy(arr, &(buffer[bufferIndex]), 2048);
        bufferIndex += 2048;
        meta->setArray(arr, 2048);

        sectDirect[i]->addItem(meta);
    }

    //Write raw NBT data
    int rawSize = base->getTransmitSize() + 1;
    char* raw = new char[rawSize];
    int index = 0;
    //cout << base->printableStr() << endl;
    base->getTransmittable(&(raw[0]), index);
    delete base;

    //Compress raw data
    uLong unCompSize = rawSize;
    uLong compSize = compressBound(unCompSize);
    char* compressed = new char[(int)compSize];
    compress((Bytef*)compressed, &compSize, (Bytef*)raw, unCompSize);

    len = compSize;
    data = new char[len];
    memcpy(data, compressed, len);

    delete raw;
    delete compressed;
}

CompressedChunk::~CompressedChunk(){
    if(data)
        delete [] data;
}

int CompressedChunk::getSizeInBlocks(){
    double totalSize = len + 5;
    totalSize /=4096;
    int result = ceil(totalSize);
    return result;
}

Chunk* CompressedChunk::getChunk(){
    uLong compSize = len;

    uLong size = 1048576; //Size of array, starts at 1mb
    uLong ucompSize = size; //Size of data in array
    char* uncompressed = new char[size];

    //Decompression is retried every time the destination buffer was too small
    //Retries are with 4x more memory than previous attempt
    //It should be very rare to need even one retry, though so the performance
    //impact is negligable

    while(true){
        if(uncompress((Bytef*)uncompressed, &ucompSize, (Bytef*) data, compSize) == Z_OK)
            break;

        if(size >= 1048576 * 256){
            cout << "Can't decompress chunk in CompressedChunk::getChunk, too big\n";
            throw -1;
        }

        //If here, destination isn't big enough. Gonna increase
        //its size and try again
        size *= 4;
        delete [] uncompressed;
        uncompressed = new char[size];
        ucompSize = size;
    }

    //At this point, uncompressed contains the proper uncompressed data
    Chunk* result = new Chunk(uncompressed, size, coord);
    delete [] uncompressed;
    return result;
}
