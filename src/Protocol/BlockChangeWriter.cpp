#include "BlockChangeWriter.h"

using namespace std;

BlockChangeWriter::BlockChangeWriter(unsigned int size) : AdvancedWriter(size){};


void BlockChangeWriter::writeBlockChanges(ChunkCoord cCoord,
        vector<Coordinate<int>>& pos, vector<Block>& blocks){

    writePacketID(0x22);
    baseThis << (int) cCoord.x;
    baseThis << (int) cCoord.z;

    int numBlocks = pos.size();
    baseThis << (short) numBlocks; //Record count
    baseThis << (int) numBlocks * 4; //Data size

    for(int i=0; i < numBlocks; i++){
        short metaAndID;
            metaAndID = (blocks[i].id & 0xFFF) << 4;
            metaAndID |= blocks[i].metadata & 0xF;

            //Taking the mod of x and z is nessecary since the & has the same effect
            unsigned char xz = (pos[i].x << 4) & 0xF0;
            xz |= pos[i].z & 0xF;

            baseThis << xz;
            baseThis << (unsigned char) pos[i].y;

            baseThis << (short) metaAndID;
    }

    addMsgLen();
}
