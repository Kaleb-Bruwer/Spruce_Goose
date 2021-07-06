#include "SuperflatGenerator.h"

#include "../World/Chunk/Chunk.h"
#include "../Datastructures/Coordinate/Coordinate.h"

SuperflatGenerator::SuperflatGenerator(BlockingQueueCounting<JobTicket*>* q)
        : GeneratorThread(q){
    layerBlocks.push_back(Block(7));
    layerHeights.push_back(1);

    layerBlocks.push_back(Block(17,2));
    layerHeights.push_back(59);

    layerBlocks.push_back(Block(3));
    layerHeights.push_back(3);

    layerBlocks.push_back(Block(2));
    layerHeights.push_back(1);
}

Chunk* SuperflatGenerator::generateChunk(ChunkCoord pos){
    Chunk* result = new Chunk(pos);

    int currY = 0;

    int numLayers = layerBlocks.size();
    for(int i=0; i<numLayers; i++){
        Coordinate<int> corner1(pos.x*16, currY, pos.z*16);
        Coordinate<int> corner2 = corner1;
        corner2.x += 15;
        corner2.z += 15;

        corner2.y += (layerHeights[i] -1);
        currY += layerHeights[i];

        result->setBlockRange(corner1, corner2, layerBlocks[i]);
    }

    return result;
}
