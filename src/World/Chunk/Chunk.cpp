#include "Chunk.h"

#include <cmath>
#include <iostream>
#include <climits>

#include "../../Entities/PlayerEntity.h"

#include "../../Datastructures/NBT/Tag_Compound.h"
#include "../../Datastructures/NBT/Tag_Byte.h"
#include "../../Datastructures/NBT/Tag_Int.h"
#include "../../Datastructures/NBT/Tag_Byte_Array.h"
#include "../../Datastructures/NBT/Tag_List.h"

#include "../../JobTickets/WorldToProtocol/BlockUpdateToPlayer.h"

using namespace std;

Chunk::Chunk(int x, int z){
    chunkCoord.x = x;
    chunkCoord.z = z;
    commonConstructor();
}

Chunk::Chunk(ChunkCoord c){
    chunkCoord = c;
    commonConstructor();
}

Chunk::Chunk(Chunk* rhs){
    chunkCoord = rhs->chunkCoord;
    for(int i=0; i<16; i++){
        if(rhs->sections[i])
            sections[i] = new ChunkSection(rhs->sections[i]);
        else
            sections[i] = 0;
    }

    for(auto it = rhs->blockData.begin(); it != rhs->blockData.end(); it++){
        BlockData* newB = it->second->clone();
        blockData[it->first] = newB;
    }
}

Chunk::Chunk(char* data, int size, ChunkCoord coord){
    chunkCoord = coord;
    commonConstructor();
    //Only receives the nbt portion of the data
    int index = 0;
    //root node is unnamed
    //Entire NBT structure deleted when scope is exited
    index++; //Otherwise it reads its own ID
    Tag_Compound nbt(data, index);

    Tag_Compound* level = (Tag_Compound*) nbt.getItem("Level");
    if(!level){
        cout << "couldn't parse chunk\n";
        return;
    }
    //read nbt->Level->Sections
    Tag_List* tagSections = (Tag_List*) level->getItem("Sections");

    int numSections = tagSections->getSize();
    for(int i=0; i<numSections; i++){
        Tag_Compound* s = (Tag_Compound*) tagSections->getValAt(i);
        int y = ((Tag_Byte*)s->getItem("Y"))->getVal();
        ChunkSection* sect = new ChunkSection();
        sections[y] = sect;

        char* blocks = ((Tag_Byte_Array*)s->getItem("Blocks"))->getArray();
        readRawBlocks(blocks, sect, i);

        char* meta = ((Tag_Byte_Array*)s->getItem("Data"))->getArray();
        readRawMeta(meta, sect);

        //Light values
    }

    //read nbt->Level->Entities
    activateChangeLog();
}

Chunk::Chunk(char* data, int &index, ChunkCoord pos, short bitmask, short addBitmask){
    chunkCoord = coord;
    commonConstructor();

    // Read Block Type
    for(int y=0; y<16; y++){
        if(bitmask & (1 << y)){
            sections[y] = new ChunkSection();
            readRawBlocks(data + index, sections[y], y);
            index += 4096;
        }
    }
    for(int y=0; y<16; y++){
        if(bitmask & (1 << y)){
            readRawMeta(data + index, sections[y]);
            index += 2048;
        }
    }

    // I realize this isn't the most effective way to skip these fields, but
    // that doesn't really matter. Plus I'll have to read them eventually anyways
    for(int y=0; y<16; y++){
        if(bitmask & (1 << y)){
            //Block light
            index += 2048;
        }
    }
    for(int y=0; y<16; y++){
        if(bitmask & (1 << y)){
            //sky light
            index += 2048;
        }
    }

    for(int y=0; y<16; y++){
        if(addBitmask & (1 << y)){
            // Add data
            index += 2048;
        }
    }

    index += 256;
}


void Chunk::readRawBlocks(char* blocks, ChunkSection* sect, int sectIndex){
    for(int b=0; b<4096; b++){
        sect->blocks[b].id = (unsigned char) blocks[b];

        //This method means I don't need to calculate excessive coordinates
        BlockData* bd = makeBlockData(blocks[b]);
        if(bd){
            Coordinate<int> pos;
            pos.y = sectIndex * 16;
            pos.y += floor(b/256);
            pos.x = b % 16;
            pos.z = (b >> 4) % 16;

            makeBlockData(pos, bd);
        }
    }
}

void Chunk::readRawMeta(char* meta, ChunkSection* sect){
    for(int b=0; b<2048; b++){
        char val = meta[b];
        sect->blocks[2*b+1].metadata = val >> 4;
        sect->blocks[2*b].metadata = val & 0xF;
    }
}

void Chunk::activateChangeLog(){
    logChanges = true;
    changes.create(this);
}

void Chunk::commonConstructor(){
    memset(sections, 0, sizeof(sections));
}

Chunk::~Chunk(){
    for(int i=0; i<numSections; i++){
        if(sections[i] != 0)
            delete sections[i];
    }
}

Block Chunk::getBlock(Coordinate<int> coord){
    int sectIndex = floor(coord.y/16);
    if(!sections[sectIndex])
        return 0;
    return sections[sectIndex]->getBlock(coord);
}

void Chunk::makeBlockData(Coordinate<int> coord, Block b){
    BlockData* bd = makeBlockData(b);
    if(bd){
        auto it = blockData.find(coord);
        if(it != blockData.end()){
            delete it->second;
            it->second = bd;
        }
        else
            blockData[coord] = bd;
    }
    else{
        //If no blockData, simply delete any existing blockData if any
        auto it = blockData.find(coord);
        if(it != blockData.end()){
            delete it->second;
            blockData.erase(it);
        }
    }
}

void Chunk::makeBlockData(Coordinate<int> coord, BlockData* bd){
    auto it = blockData.find(coord);
    if(it != blockData.end()){
        delete it->second;
        it->second = bd;
    }
    else
        blockData[coord] = bd;
}

void Chunk::setBlock(Coordinate<int> coord, Block block){
    makeBlockData(coord, block);
    int sectIndex = floor(coord.y/16);
    if(!sections[sectIndex]){
        sections[sectIndex] = new ChunkSection();
    }
    sections[sectIndex]->setBlock(coord, block);

    if(logChanges){
        changes.addChange(coord, block);
    }
}

void Chunk::setBlockRange(Coordinate<int> coord1, Coordinate<int> coord2, Block block){
    int lowestSect = floor(coord1.y/16);
    int highestSect = floor(coord2.y/16);

    if(logChanges){ //NOT used during world generation, this is for later changes
        Coordinate<int> lowCorner;
        Coordinate<int> highCorner;
        lowCorner.x = min(coord1.x, coord2.x);
        lowCorner.y = min(coord1.y, coord2.y);
        lowCorner.z = min(coord1.z, coord2.z);

        highCorner.x = max(coord1.x, coord2.x);
        highCorner.y = max(coord1.y, coord2.y);
        highCorner.z = max(coord1.z, coord2.z);

        BlockData* bd = makeBlockData(block);
        bool needBD = false;
        if(bd){
            needBD = true;
            delete bd;
            bd = 0;
        }

        for(int x= lowCorner.x; x <= highCorner.x; x++){
            for(int y = lowCorner.y; y <= highCorner.y; y++){
                for(int z = lowCorner.z; z <= highCorner.z; z++){
                    if(needBD){
                        makeBlockData(Coordinate<int>(x,y,z), block);
                    }
                    changes.addChange(Coordinate<int>(x,y,z), block);
                }
            }
        }
    }

    if(lowestSect > highestSect){
        int temp = lowestSect;
        lowestSect = highestSect;
        highestSect = temp;
    }

    for(int i= lowestSect; i <= highestSect; i++){
        if(!sections[i])
            sections[i] = new ChunkSection();
    }

    if(lowestSect == highestSect){
        sections[lowestSect]->setBlockRange(coord1, coord2, block);
    }
    else{
        Coordinate<int> newTop(&coord2);
        newTop.y = lowestSect*16 + 15;
        sections[lowestSect]->setBlockRange(coord1, newTop, block);

        Coordinate<int> newBottom(&coord1);
        newBottom.y = newTop.y + 1;
        newTop.y +=16;
        for(int i= lowestSect+1; i<highestSect; i++){
            sections[i]->setBlockRange(newBottom, newTop, block);
            newBottom.y+=16;
            newTop.y+=16;
        }

        sections[highestSect]->setBlockRange(newBottom, coord2, block);
    }
}

void Chunk::startBreak(int nTicks, Coordinate<int> pos){
    pos.insideChunk();

    if(nTicks == 0){
        //immediately break block, don't add to pendingBreaks
        setBlock(pos, Block(0));
    }
    else{
        //first check for duplicates
        for(auto it = pendingBreaks.begin(); it != pendingBreaks.end(); it++){
            if(it->second == pos){
                if(it->first > nTicks){
                    pendingBreaks.erase(it); //Keeps one with the least time left
                }
                break;
            }
        }

        //Add to pendingBreaks in order of nTicks
        pair<int, Coordinate<int>> val(nTicks, pos);
        if(pendingBreaks.empty()){
            pendingBreaks.push_back(val);
        }
        else{
            for(auto it = pendingBreaks.begin(); it != pendingBreaks.end(); it++){
                auto next = it;
                next++;

                if(next == pendingBreaks.end() || next->first >= nTicks){
                    pendingBreaks.insert(it, val);
                }
            }
        }
    }
}

int Chunk::breakTicksLeft(Coordinate<int> pos){
    pos.insideChunk();
    for(auto it = pendingBreaks.begin(); it != pendingBreaks.end(); it++){
        if(it->second == pos){
            int nTicks = it->first;
            return nTicks;
        }
    }
    return INT_MAX;
}

void Chunk::checkBreaks(){
    //Decrements nTicks on all pendingBreaks
    //Deletes pendingBreaks that are overdue
    const int maxOver = -5;
    for(auto it = pendingBreaks.begin(); it != pendingBreaks.end();){
        it->first--;

        if(it->first < maxOver){
            it = pendingBreaks.erase(it);
        }
        else{
            it++;
        }
    }
}

BlockData* Chunk::getBlockData(Coordinate<int> pos){
    auto it = blockData.find(pos);
    if(it == blockData.end())
        return 0;
    else
        return it->second;
}

void Chunk::writeBlockIDs(unsigned char* buffer){ //size = 65536 = 16 * 4096
    int index = 0;
    for(int s = 0; s<16; s++){
        if(sections[s]){
            for(int i=0; i < 4096; i++){
                buffer[index] = (unsigned char) sections[s]->blocks[i].id;
                index++;
            }
        }
        else{
            memset(&(buffer[index]), 0, 4096);
            index += 4096;
        }
    }
}

bool Chunk::writeBlockIDs(unsigned char* buffer, int s){
    if(!sections[s])
        return false;

    int index = 0;
    ChunkSection* sect = sections[s];
    for(int i=0; i < 4096; i++){
        buffer[index] = (unsigned char) sect->blocks[i].id;
        index++;
    }
    return true;
}

void Chunk::writeMetaData(unsigned char* buffer){ //size = 32768 = 16 * 2048
    int index = 0;
    for(int s = 0; s<16; s++){
        if(sections[s]){
            for(int i=0; i < 2048; i++){
                unsigned char val = 0;
                val = (sections[s]->blocks[i*2+1].metadata << 4);
                val |= (sections[s]->blocks[i*2].metadata & 0xF);
                buffer[index] = val;
                index++;
            }
        }
        else{
            memset(&(buffer[index]), 0, 2048);
            index += 2048;
        }
    }
}

bool Chunk::writeMetaData(unsigned char* buffer, int s){
    if(!sections[s])
        return false;

    int index = 0;
    for(int i=0; i < 2048; i++){
        unsigned char val = 0;
        val = (sections[s]->blocks[i*2+1].metadata << 4);
        val |= (sections[s]->blocks[i*2].metadata & 0xF);
        buffer[index] = val;
        index++;
    }
    return true;
}

void Chunk::writeLightVals(unsigned char* buffer){ //size = 32768 = 16 * 2048
    memset(&(buffer[0]), 15, 32768);
}

bool Chunk::writeLightVals(unsigned char* buffer, int s){
    if(!sections[s])
        return false;

    memset(&(buffer[0]), 255, 2048);
    return true;
}

unsigned short Chunk::getSectionBitMask(){
    unsigned short result = 0;
    unsigned short uno = 1;
    for(int i=0; i<16; i++){
        if(sections[i]){
            result |= uno;
        }
        uno = uno << 1;
    }
    return result;
}
