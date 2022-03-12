#ifndef PACFULLCHUNK1_16_3_H
#define PACFULLCHUNK1_16_3_H

#include "PacFullChunk.h"

using namespace std;

class PacFullChunk1_16_3 : public PacFullChunk{
protected:
    void writeFullChunk(Chunk* c, PacketWriter &pw);
public:
    PacFullChunk1_16_3(PlayerConnection* p): PacFullChunk(p){};

    void handle(Chunk* chunk);
};

#endif
