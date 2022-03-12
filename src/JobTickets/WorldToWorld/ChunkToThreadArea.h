#ifndef CHUNKTOTHREADAREA_H
#define CHUNKTOTHREADAREA_H

#include "../JobTicket.h"

using namespace std;

class ChunkToThreadArea : public JobTicket{
public:
    ChunkToThreadArea() : JobTicket(CHUNKTOTHREADAREA){};
    Chunk* chunk = 0;
};

#endif
