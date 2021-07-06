#ifndef CHUNKTOTHREADAREA_H
#define CHUNKTOTHREADAREA_H

#include "../JobTicket.h"

using namespace std;

class ChunkToThreadArea : public JobTicket{
public:
    Chunk* chunk = 0;

    string getType(){return "ChunkToThreadArea";};
};

#endif
