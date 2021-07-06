#include "TerrainGen1.h"

#include <cmath>

#include "../World/Chunk/Chunk.h"
#include "../Datastructures/Coordinate/Coordinate.h"

TerrainGen1::TerrainGen1(BlockingQueueCounting<JobTicket*>* q, int s)
        : GeneratorThread(q){
    seed = s;
}

Chunk* TerrainGen1::generateChunk(ChunkCoord cCoord){
    PerlinNoise noise(seed, 64, 16);
    int frameX, frameZ;
    frameX = floor((double)cCoord.x/4);
    frameZ = floor((double)cCoord.z/4);

    int xOffset = cCoord.x % 4;
    if(xOffset < 0)
        xOffset += 4;
    int zOffset = cCoord.z % 4;
    if(zOffset < 0)
        zOffset += 4;

    noise.specifySubframe(xOffset * 16, zOffset * 16, 16);
    noise.generateFrame(frameX, frameZ);

    Chunk* result = new Chunk(cCoord);

    //Set bedrock
    result->setBlockRange(Coordinate<int>(0,0,0),Coordinate<int>(15, 0, 15) , Block(7));

    int height;
    for(int x=0; x<16; x++){
        for(int z=0; z<16; z++){
            height = 40 + floor(noise.getVal(x,z));
            height = min(height, 200);
            height = max(height, 0);

            Coordinate<int> b1(x, 1, z);
            Coordinate<int> b2(x, height, z);
            result->setBlockRange(b1,b2, Block(1));

            b1 = b2;
            b1.y++;

            b2 = b1;
            b2.y += 3;
            result->setBlockRange(b1,b2, Block(3));

            b2.y++;
            result->setBlock(b2, Block(2));
        }
    }
    return result;
}
