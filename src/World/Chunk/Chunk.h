#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <map>
#include <list>
#include <utility>

#include "../../JobTickets/JobTicket.h"

#include "../../Datastructures/NBT/Tag_Compound.h"
#include "../../Datastructures/NBT/Tag_Long_Array.h"
#include "ChunkSection.h"
#include "ChunkChangeTracker.h"
#include "../../Inventory/BlockData/BlockData.h"

#include "ChestDoubleWrapper.h"

class ThreadArea;

//IMPORTANT: if logChanges==false, then players will not see each other's
//block changes.

/*
Planning on removing ChunkSection entirely when I rework the Chunk class, so the
current state of this class is meant to be a temporary solution
*/

class Chunk{
private:
    void readRawBlocks(char* data, ChunkSection* sect, int sectIndex);
    void readRawMeta(char* data, ChunkSection* sect);

protected:
    const int numSections = 16; //In case height limit changes (I hope it does)
    ChunkCoord chunkCoord;
    ChunkSection* sections[16];

    bool logChanges = false; //Activated AFTER world generation
    ChunkChangeTracker changes;
    list<pair<int, Coordinate<int>>> pendingBreaks;
    map<Coordinate<int>, BlockData*> blockData;
    map<Coordinate<int>, ChestDoubleWrapper*> doubleChests;

    void commonConstructor();

    //Handles entire deal
    void makeBlockData(Coordinate<int>, Block b);
    void makeBlockData(Coordinate<int>, BlockData* bd);

    void combineDoubleChests();

    friend class ThreadArea;
public:
    Chunk(int x, int y);
    Chunk(ChunkCoord c);
    Chunk(Chunk*); //Used for FullChunk packets
    Chunk(char* data, int size, ChunkCoord pos); //Uncompressed chunk data from file
    Chunk(char* data, int &index, ChunkCoord pos, short bitmask, short addBitmask); //Uncompressed chunk data from protocol
    ~Chunk();

    ChunkCoord getChunkCoord(){
        return chunkCoord;
    };

    Block getBlock(Coordinate<int> coord);
    void setBlock(Coordinate<int> coord, Block block);
    void setBlockRange(Coordinate<int> coord1, Coordinate<int> coord2, Block block);

    void activateChangeLog();
    void sendChanges(){changes.sendChanges();};
    void addPlayer(PlayerEntity* p){changes.addPlayer(p);};
    void removePlayer(PlayerEntity* p){changes.removePlayer(p);};
    bool hasPlayers(){return changes.hasPlayers();};

    void startBreak(int nTicks, Coordinate<int> block);
    void checkBreaks();
    int breakTicksLeft(Coordinate<int> block);

    void writeBlockIDs(unsigned char*);//size = 65536 = 16 * 4096
    void writeMetaData(unsigned char*);//size = 32768 = 16 * 2048
    void writeLightVals(unsigned char*);//size = 32768 = 16 * 2048

    //These versions are per-section
    //return value indicates if sections[s] exists
    bool writeBlockIDs(unsigned char*, int s); //size = 4096
    bool writeMetaData(unsigned char*, int s); //size = 2048
    bool writeLightVals(unsigned char*, int s); //size = 2048

    unsigned short getSectionBitMask();

    BlockData* getBlockData(Coordinate<int> pos);

};

#endif
