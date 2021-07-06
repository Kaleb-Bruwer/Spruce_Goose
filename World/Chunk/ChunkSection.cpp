#include "ChunkSection.h"

#include <cstring> //memcpy
#include <cmath>
#include <iostream>

#include "../../Protocol/ProtocolHelpers.h" //switchEndian
#include "../../Datastructures/Varint.h"

using namespace std;

ChunkSection::ChunkSection(){

}

ChunkSection::ChunkSection(ChunkSection* rhs){
    nonAirCount = rhs->nonAirCount;
    memcpy(blocks, rhs->blocks, sizeof(blocks));

}

ChunkSection::~ChunkSection(){}

Block ChunkSection::getBlock(Coordinate<int> coord){
    coord.insideChunkSection();
    int index = ((coord.y*sectionW) + coord.z)* sectionW + coord.x;
    return blocks[index];
}

void ChunkSection::setBlock(Coordinate<int> coord, Block block){
    coord.insideChunkSection(); //Can not be out of bounds any more
    int index = ((coord.y*sectionW) + coord.z)* sectionW + coord.x;

    if(blocks[index].id == 0 && block.id != 0)
        nonAirCount++;
    else if(blocks[index].id != 0 && block.id == 0)
        nonAirCount++;

    blocks[index] = block;
}

void ChunkSection::setBlockRange(Coordinate<int> coord1, Coordinate<int> coord2, Block block){
    coord1.insideChunkSection();
    coord2.insideChunkSection();

    Coordinate<int> coordTemp;
    coordTemp.x = min(coord1.x, coord2.x);
    coordTemp.y = min(coord1.y, coord2.y);
    coordTemp.z = min(coord1.z, coord2.z);

    coord2.x = max(coord1.x, coord2.x);
    coord2.y = max(coord1.y, coord2.y);
    coord2.z = max(coord1.z, coord2.z);

    coord1.x = coordTemp.x;
    coord1.y = coordTemp.y;
    coord1.z = coordTemp.z;

    int zJump = sectionW - (coord2.x - coord1.x) -1;
    int yJump = sectionW*sectionW - sectionW*(coord2.z -coord1.z +1);

    int airInSelection = 0;

    int index = ((coord1.y*sectionW) + coord1.z)* sectionW + coord1.x;
    for(int y= coord1.y; y <= coord2.y; y++){
        for(int z = coord1.z; z <= coord2.z; z++){
            for(int x = coord1.x; x <= coord2.x; x++){
                if(blocks[index].id == 0)
                    airInSelection++;
                blocks[index] = block;
                index++;
            }
            index += zJump;
        }
        index += yJump;
    }

    if(block.id != 0){
        nonAirCount+= airInSelection;
    }
    else{
        int selectSize = (coord2.x - coord1.x + 1);
        selectSize *= (coord2.y - coord1.y + 1);
        selectSize *= (coord2.z - coord1.z + 1);

        nonAirCount -= (selectSize - airInSelection);
    }
}

bool ChunkSection::fillHeightMap(unsigned short* heightMap, int sectionPos){
    int blockIndex = sectionSize-1;
    int hMIndex; //heightMap index
    sectionPos *= 16;
    //h is actual height in chunk
    for(int h=sectionH-1 + sectionPos; h>=sectionPos; h--){
        hMIndex = 255;
        for(int z=15; z>=0; z--){
            for(int x=15; x>=0; x--){
                if(!heightMap[hMIndex])
                    if(blocks[blockIndex].id)
                        heightMap[hMIndex] = h;
                hMIndex--;
                blockIndex--;
            }
        }
    }
}
