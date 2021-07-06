#include "CoordAreaPair.h"

using namespace std;

CoordAreaPair::CoordAreaPair(ChunkCoord c, ThreadArea* t){
    chunkCoord = c;
    threadArea = t;
}

CoordAreaPair& CoordAreaPair::operator=(CoordAreaPair& rhs){
    chunkCoord = rhs.chunkCoord;
    threadArea = rhs.threadArea;

    return *this;
}
