#ifndef MOVENTITIESTOSPLITF_H
#define MOVENTITIESTOSPLITF_H

#include "../../General/Functor.h"
#include "../../Entities/Entity.h"

using namespace std;

class ThreadArea;

class MovEntitiesToSplitF : public Functor<Entity*>{
protected:
    ThreadArea* dest;
public:
    MovEntitiesToSplitF(ThreadArea* d);

    void operator()(Entity* e);
};

#endif
