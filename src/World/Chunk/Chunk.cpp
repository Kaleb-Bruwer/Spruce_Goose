#include "Chunk.h"

#include <cmath>
#include <iostream>
#include <climits>
#include <set>

#include "../../Entities/PlayerEntity.h"

#include "../../Datastructures/NBT/Tag_Compound.h"
#include "../../Datastructures/NBT/Tag_Byte.h"
#include "../../Datastructures/NBT/Tag_Int.h"
#include "../../Datastructures/NBT/Tag_Byte_Array.h"
#include "../../Datastructures/NBT/Tag_List.h"

#include "../../Inventory/BlockData/BaseChest.h"
#include "../../Inventory/BlockData/CraftingTable.h"

#include "../../Entities/Item.h"

#include "../../JobTickets/WorldToProtocol/BlockUpdateToPlayer.h"

using namespace std;

//Creates the appropriate BlockData object for provided block
//returns 0 if none needed
BlockData* makeBD(Block b){
    switch(b.id){
        case 54:
        case 146:
            return new ChestSingle();
        case 58:
            return new CraftingTable();
        case 61:
        case 62:
            //furnace
        case 23:
        case 158:
            //Dispenser / dropper
        case 116:
            //enchantment table
            break;
    }
    //Most cases will be a 0
    return 0;
};

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

    for(auto it = rhs->doubleChests.begin(); it != rhs->doubleChests.end(); it++){
        ChestDoubleWrapper* newB = it->second->clone();
        doubleChests[it->first] = newB;
    }

}

//Uncompressed chunk data from file
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
    combineDoubleChests();

    //read nbt->Level->Entities
    activateChangeLog();
}

//Uncompressed chunk data from protocol
Chunk::Chunk(char* data, int &index, ChunkCoord pos, short bitmask, short addBitmask){
    chunkCoord = pos;
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
    combineDoubleChests();

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

    // I forgot this ONE LINE and wasted days figuring it out
    activateChangeLog();
}


void Chunk::readRawBlocks(char* blocks, ChunkSection* sect, int sectIndex){
    for(int b=0; b<4096; b++){
        sect->blocks[b].id = (unsigned char) blocks[b];

        //This method means I don't need to calculate excessive coordinates
        BlockData* bd = makeBD(Block(blocks[b]));
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

    // TODO missing a bunch of stuff

    for(auto it = blockData.begin(); it != blockData.end(); it++){
        delete it->second;
    }

    // The set prevents double deletes
    set<ChestDoubleWrapper*> doubles;
    for(auto it = doubleChests.begin(); it != doubleChests.end(); it++){
        doubles.insert(it->second);
    }

    for(auto it = doubles.begin(); it != doubles.end(); it++){
        delete *it;
    }

}

Block Chunk::getBlock(Coordinate<int> coord){
    int sectIndex = floor(coord.y/16);
    if(!sections[sectIndex])
        return 0;
    return sections[sectIndex]->getBlock(coord);
}

void Chunk::makeChest(Coordinate<int> coord, ChestSingle* chest){
    // Check for an adjacent chest
    Coordinate<int> pos = coord;
    bool found = false;
    bool lower;
    Coordinate<int> target;


    pos.x--;
    auto it = blockData.find(pos);
    if(it != blockData.end()){
        target = pos;
        found = true;
        lower = true;
    }
    else{
        pos.x++;
        pos.z--;
        auto it = blockData.find(pos);
        if(it != blockData.end()){
            target = pos;
            found = true;
            lower = true;
        }
        else{
            pos.z += 2;
            auto it = blockData.find(pos);
            if(it != blockData.end()){
                target = pos;
                found = true;
                lower = false;
            }
            else{
                pos.z--;
                pos.x++;
                auto it = blockData.find(pos);
                if(it != blockData.end()){
                    target = pos;
                    found = true;
                    lower = false;
                }
            }
        }
    }

    if(found){
        // Found an adjacent chest
        ChestDoubleWrapper* wrap = new ChestDoubleWrapper();
        ChestDouble* cd;

        if(lower){
            wrap->chest = new ChestDouble(*((ChestSingle*) it->second), *(chest));
            wrap->pos1 = target;
            wrap->pos2 = coord;
        }
        else{
            wrap->chest = new ChestDouble(*chest, *((ChestSingle*) it->second));
            wrap->pos1 = coord;
            wrap->pos2 = target;
        }
        delete it->second;
        delete chest;
        blockData.erase(it);
        doubleChests[wrap->pos1] = wrap;
        doubleChests[wrap->pos2] = wrap;
        return;
    }
    else{
        // If here, it's not a double chest
        auto it = blockData.find(coord);
        if(it != blockData.end()){
            delete it->second;
            it->second = chest;
        }
        else
            blockData[coord] = chest;
    }
}

void Chunk::makeBlockData(Coordinate<int> coord, Block b){
    BlockData* bd = makeBD(b);
    if(bd){
        if(bd->getType() == CHESTSINGLE){
            makeChest(coord, (ChestSingle*) bd);
            return;
        }

        // If here, it's not a double chest
        auto it = blockData.find(coord);
        if(it != blockData.end()){
            delete it->second;
            it->second = bd;
        }
        else
            blockData[coord] = bd;
    }
    else{
        //If no blockData, simply delete any existing blockData
        // DOESN'T CHECK FOR DOUBLE CHESTS
        auto it = blockData.find(coord);
        if(it != blockData.end()){
            delete it->second;
            blockData.erase(it);
        }
    }
}

void Chunk::makeBlockData(Coordinate<int> coord, BlockData* bd){
    if(bd->getType() == CHESTSINGLE){
        makeChest(coord, (ChestSingle*) bd);
        return;
    }

    auto it = blockData.find(coord);
    if(it != blockData.end()){
        delete it->second;
        it->second = bd;
    }
    else
        blockData[coord] = bd;
}

void Chunk::combineDoubleChests(){
    for(int s=0; s<16; s++){
        ChunkSection* cs = sections[s];
        if(!cs)
            continue;

        Coordinate<int> base(chunkCoord.x * 16, s * 16, chunkCoord.z * 16);
        for(int y=0; y<16; y++){
            for(int z=0; z<16; z++){
                for(int x=0; x<16; x++){
                    Coordinate<int> pos = base;
                    pos.x += x;
                    pos.y += y;
                    pos.z += z;
                    auto bd = blockData.find(pos);
                    if(bd == blockData.end())
                        continue;

                    if(bd->second->getType() == CHESTSINGLE){
                        // Check one up on x and y axes for another CHESTSINGLE
                        Coordinate<int> p1 = pos;
                        pos.x++;

                        auto bd2 = blockData.find(pos);
                        if(bd == blockData.end()){
                            // Check at z + 1
                            pos.x--;
                            pos.z++;

                            bd2 = blockData.find(pos);

                        }

                        if(bd != blockData.end()){
                            // Match has been found
                            ChestDoubleWrapper* wrap = new ChestDoubleWrapper();
                            ChestSingle* c1 = (ChestSingle*) bd->second;
                            ChestSingle* c2 = (ChestSingle*) bd2->second;

                            wrap->chest = new ChestDouble(*c1, *c2);
                            wrap->pos1 = p1;
                            wrap->pos2 = pos;

                            delete bd->second;
                            delete bd2->second;

                            blockData.erase(bd);
                            blockData.erase(bd2);

                            doubleChests[p1] = wrap;
                            doubleChests[pos] = wrap;
                        }
                    }

                }
            }
        }
    }
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

vector<Item*> Chunk::breakBlock(Coordinate<int> coord, Slot tool){
    ChunkSection* sect = sections[(int)floor(coord.y/16)];
    if(!sect)
        return vector<Item*>();

    vector<Item*> drops;
    Slot drop0 = sect->getBlock(coord).getDrop(tool);
    if(!drop0.isEmpty()){
        Item* item = new Item(drop0);
        item->setPos(coord);
        drops.push_back(item);
    }

    // If block contained items, those must be dropped
    BlockData* bd = getBlockData(coord);
    if(bd){
        switch(bd->getType()){
        case CHESTDOUBLE:{
            ChestDouble* c = (ChestDouble*) bd;
            // First split into two single chests, one of which will survive
            ChestSingle* single1 = new ChestSingle();
            ChestSingle* single2 = new ChestSingle();

            // single1 is first half of chest
            c->splitChest(*single1, *single2);

            // Can be assumed to exist since CHESTDOUBLE was returned by getBlockData
            ChestDoubleWrapper* wrap = doubleChests[coord];
            doubleChests.erase(wrap->pos1);
            doubleChests.erase(wrap->pos2);

            ChestSingle* broke;

            if(wrap->pos1 == coord){
                blockData[wrap->pos2] = single2;
                broke = single1;
            }
            else{
                blockData[wrap->pos1] = single1;
                broke = single2;
            }

            for(int i=0; i<27; i++){
                if(!broke->slots[i].isEmpty()){
                    Item* item = new Item(broke->slots[i]);
                    item->setPos(coord);
                    drops.push_back(item);
                }
            }
            delete wrap;
            delete broke;

            break;
        }
        case CHESTSINGLE:{
            ChestSingle* c = (ChestSingle*) bd;
            for(int i=0; i<27; i++){
                if(!c->slots[i].isEmpty()){
                    Item* item = new Item(c->slots[i]);
                    item->setPos(coord);
                    drops.push_back(item);
                }
            }
            break;
        }
        }
    }


    return drops;
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

        BlockData* bd = makeBD(block);
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
            return it->first;
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
    if(it != blockData.end())
        return it->second;

    auto it2 = doubleChests.find(pos);
    if(it2 != doubleChests.end()){
        return it2->second->chest;
    }
    return 0;
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
