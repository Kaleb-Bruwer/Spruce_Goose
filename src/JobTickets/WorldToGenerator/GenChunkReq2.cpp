#include "GenChunkReq2.h"

using namespace std;

void GenChunkReq2::removeChunk(ChunkCoord c){
    for(int i=0; i<chunks.size(); i++){
        if(chunks[i] == c){
            chunks.erase(chunks.begin() + i);
            return;
        }
    }
}


bool GenChunkReq2::contains(ChunkCoord c){
    for(int i=0; i<chunks.size(); i++){
        if(chunks[i] == c){
            return true;
        }
    }
    return false;
}
