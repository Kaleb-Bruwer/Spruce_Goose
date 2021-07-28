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
    cout << "!!HEADER!!\n";
    for(int i=0; i<7; i++){
        cout << (int)(unsigned char)buffer[index + i] << " ";
    }
    cout << "\n\n";

    short int numChunks = readShort();
    int dataLen = readInt();
    bool skylightSent = readBool();

    char* data = new char[dataLen];
    readSegment(dataLen, data);

    cout << "!!DATA!!\n";
    for(int i=0; i<dataLen; i++){
        cout << (int)(unsigned char)data[i] << " ";
    }
    cout << "\n\n";

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

    cout << "!!META!!\n";
    for(int i=0; i<metaLen; i++){
        cout << (int)(unsigned char)meta[i] << " ";
    }
    cout << endl;

    cout << "MapChunkBulk\n";
    cout << "\tnum chunks: " << numChunks << endl;
    cout << "\tdata len: " << dataLen << endl;
    cout << "\tmeta len: " << metaLen << endl;

    delete [] data;
    delete [] meta;
}
