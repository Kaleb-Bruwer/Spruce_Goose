#include "MapChunkBulkReader.h"

#include "../../Protocol/ProtocolHelpers.h"

#include <vector>

using namespace std;

MapChunkBulkReader::MapChunkBulkReader(PacketReader &p){
    size = p.size;
    index = p.index;
    buffer = p.buffer;

    ownData = false;
}

MapChunkBulkReader::~MapChunkBulkReader(){
    // Prevents deletion in PacketReader destructor
    if(!ownData)
        buffer = 0;
}


void MapChunkBulkReader::readAll(int end){
    short int numChunks = readShort();
    int dataLen = readInt();
    bool skylightSent = readBool();

    char* data = new char[dataLen];
    readSegment(dataLen, data);

    //using lenRemaining here would include packets after this one
    // each chunk has 12 bytes of metadata
    int metaLen = 12 * numChunks;
    char* meta = new char[metaLen];
    readSegment(metaLen, meta);

    vector<RawChunkMeta> metaStructs(numChunks);
    for(int i=0; i<numChunks; i++){
        memcpy(&metaStructs[i].coord.x, &(meta[12*i]),4);
        switchEndian(&metaStructs[i].coord.x, 4);

        memcpy(&metaStructs[i].coord.z, &(meta[12*i + 4]),4);
        switchEndian(&metaStructs[i].coord.z, 4);

        memcpy(&metaStructs[i].bitmask, &(meta[12*i + 8]),2);
        switchEndian(&metaStructs[i].bitmask, 2);

        memcpy(&metaStructs[i].addBitmask, &(meta[12*i + 10]),2);
        switchEndian(&metaStructs[i].addBitmask, 2);
    }

    delete [] data;
    delete [] meta;
}
