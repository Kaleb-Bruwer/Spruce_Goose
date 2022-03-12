#ifndef CHUNKCHECKBREAKSF_H
#define CHUNKCHECKBREAKSF_H

#include "../../General/Functor.h"

#include "../Chunk/Chunk.h"

using namespace std;

class ChunkCheckBreaksF : public Functor<Chunk*>{
public:

    void operator()(Chunk* c){
        c->checkBreaks();
    };
};

#endif
