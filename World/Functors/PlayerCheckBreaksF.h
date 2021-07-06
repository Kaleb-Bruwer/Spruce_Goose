#ifndef PLAYERCHECKBREAKS_H
#define PLAYERCHECKBREAKS_H

#include "../../General/Functor.h"

#include "../Chunk/Chunk.h"

using namespace std;

class PlayerEntity;
class ThreadArea;

class PlayerCheckBreaksF : public Functor<PlayerEntity*>{
private:
    ThreadArea* area;
public:
    PlayerCheckBreaksF(ThreadArea* t){
        area = t;
    }
    void operator()(PlayerEntity* p);
};

#endif
