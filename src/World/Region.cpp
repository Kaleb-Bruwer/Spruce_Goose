#include "Region.h"

#include <fstream>
#include <cstring>

#include "../Protocol/ProtocolHelpers.h" //switchEndian()

using namespace std;

void Region::commonConstructor(){
    memset(chunks, 0, sizeof(chunks));
    memset(compressed, 0, sizeof(compressed));
}

string Region::coordToFileName(RegionCoord r){
    return "r." + to_string(r.x) + "." + to_string(r.z) + ".mca";
}

string Region::getFileNameDir(RegionCoord r){
    string dir = "Save/region/";//TODO: get directory
    return dir + coordToFileName(r);
}

string Region::getFileNameDir(){
    return getFileNameDir(rCoord);
}

Region::Region(){
    commonConstructor();

}

Region::Region(RegionCoord r){
    rCoord = r;
    commonConstructor();
    read(r);
}

bool Region::read(RegionCoord r){
    rCoord = r;
    string filename = getFileNameDir(r);

    ifstream file(filename);
    if(!file.good())
        return false;

    int filePos = 0; //Steps of 4096 bytes

    //Read location table
    char locationRaw[4096];
    file.read(&(locationRaw[0]),4096);

    int numChunks = 0;
    fileLocation locationTable[1024];

    int z = 0;
    int x = -1;

    for(int i = 0; i<1024; i++){
        x++;
        if(x == 32){
            z++;
            x = 0;
        }

        unsigned int val;
        memcpy(&val, &(locationRaw[i*4]), 4);
        if(val == 0)
            continue;
        switchEndian((void*)&val,4);

        int size = val &0xFF;
        int offset = val >> 8;
        locationTable[numChunks].offset = offset;
        locationTable[numChunks].size = size;
        locationTable[numChunks].x = x;
        locationTable[numChunks].z = z;
        numChunks++;
    }

    //Sort locationTable by offset from smallest to largest, but with 0's at back

    for(int i=0; i<numChunks-1; i++){
        unsigned int smallestIndex = i;
        unsigned int smallestOffset = locationTable[i].offset;

        for(int j=i+1; j<numChunks; j++){
            if(locationTable[j].offset < smallestOffset){
                smallestIndex = j;
                smallestOffset = locationTable[j].offset;
            }
        }
        if(smallestIndex != i){
            fileLocation temp;
            temp = locationTable[i];

            locationTable[i] = locationTable[smallestIndex];
            locationTable[smallestIndex] = temp;
        }
    }

    //Timestamps are currently ignored
    file.ignore(4096);
    filePos += 2;


    const int bufferSize = 3;
    char buffer[4096 * bufferSize];

    ChunkCoord baseCoord;
    baseCoord.x = r.x*32;
    baseCoord.z = r.z*32;

    for(int pos = 0; pos <numChunks; pos++){
        if(locationTable[pos].size == 0)
            break;

        int size = locationTable[pos].size;
        int offset = locationTable[pos].offset;
        x = locationTable[pos].x;
        z = locationTable[pos].z;

        //Read chunk
        //First find right start position
        int gap = offset - filePos;
        if(gap > 0){
            file.ignore(gap * 4096);
            filePos +=gap;
        }
        else if(gap < 0){
            cout << "File screwed\n";
            return false;
        }

        ChunkCoord coord = baseCoord;
        coord.x += x;
        coord.z += z;

        //Buffer used whenever chunk fits in it, this significantly reduces
        //memory allocations to save time
        if(size > bufferSize){
            char* chunkLanding = new char[4096 * size];
            file.read(chunkLanding, 4096*size);
            filePos += size;
            compressed[z][x] = new CompressedChunk(chunkLanding, coord);
            delete [] chunkLanding;
        }
        else{
            file.read(&(buffer[0]), 4096*size);
            filePos += size;
            compressed[z][x] = new CompressedChunk(&(buffer[0]), coord);
        }
    }
    file.close();

    return true;
}

void Region::write(){
    //Build locationTable
    fileLocation locationTable[1024];
    memset((void*)&(locationTable[0]), 0, sizeof(locationTable));

    int index = 0;
    int offset = 2;
    for(int z= 0; z<32; z++){
        for(int x = 0; x<32; x++){
            if(!compressed[z][x]){
                index++;
                continue;
            }
            locationTable[index].size = compressed[z][x]->getSizeInBlocks();
            locationTable[index].offset = offset;
            locationTable[index].x = x;
            locationTable[index].z = z;
            offset += locationTable[index].size;
            index++;
        }
    }

    int totalSize = offset;
    index = 0;
    char* buffer = new char[totalSize*4096];
    memset(buffer, 0, totalSize*4096);

    //Write locationTable to buffer
    for(int i=0; i<1024; i++){
        int val = locationTable[i].offset << 8;
        val |=  locationTable[i].size & 0xFF;
        switchEndian(&val, 4);
        memcpy(&(buffer[index]), &val, 4);
        index += 4;
    }

    //Write last access table
    memset(&(buffer[index]), 0, 4096);
    index += 4096;

    //Write actual chunks
    for(int i=0; i<1024; i++){
        if(locationTable[i].size == 0)
            continue;
        int x = locationTable[i].x;
        int z = locationTable[i].z;
        ChunkCoord c = compressed[z][x]->coord;
        unsigned int size = compressed[z][x]->len;
        switchEndian((void*)&size, 4);
        memcpy(&(buffer[index]), (void*)&size, 4);
        size = compressed[z][x]->len;
        buffer[index+4] = 2;
        memcpy(&buffer[index+5], compressed[z][x]->data, size);
        index += 4096 * locationTable[i].size;
    }
    ofstream file(getFileNameDir());
    file.write(buffer, index);
    file.close();

}

//It is assumed that this will only be chunks that didn't already exist,
//since that would be a bug in and of itself
//(These come from the World Generator)
void Region::addChunk(Chunk* chunk){
    ChunkCoord c = chunk->getChunkCoord();
    int x = c.x %32;
    if(x < 0)
        x += 32;
    int z = c.z %32;
    if(z < 0)
        z += 32;

    if(chunks[z][x] && chunks[z][x] != chunk){
        delete chunks[z][x];
        chunks[z][x] = chunk;
    }

    if(compressed[z][x])
        delete compressed[z][x];
    compressed[z][x] = new CompressedChunk(chunk);
}

void Region::addChunk(CompressedChunk* chunk){
    ChunkCoord c = chunk->coord;
    int x = c.x %32;
    if(x < 0)
        x += 32;
    int z = c.z %32;
    if(z < 0)
        z += 32;

    if(compressed[z][x])
        delete compressed[z][x];

    compressed[z][x] = chunk;
}

Chunk* Region::getChunk(ChunkCoord c){
    int x = c.x %32;
    if(x < 0)
        x += 32;
    int z = c.z %32;
    if(z < 0)
        z += 32;

    if(chunks[z][x])
        return chunks[z][x];

    if(compressed[z][x]){
        chunks[z][x] = compressed[z][x]->getChunk();
        return chunks[z][x];
    }

    //If you got this far, the chunk has not been generated yet
    //Blocking on this would waste a lot of time, so SynchedArea
    //Will receive 0 and take it from there
    //SynchedArea will send a JobTicket to WorldGenerator & receive Chunk.
    return 0;
}
