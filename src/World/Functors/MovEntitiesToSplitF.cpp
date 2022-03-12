#include "MovEntitiesToSplitF.h"

#include <iostream>

#include "../ThreadArea.h"

using namespace std;

MovEntitiesToSplitF::MovEntitiesToSplitF(ThreadArea* d){
    dest = d;
}

void MovEntitiesToSplitF::operator()(Entity* e){
    dest->addEntity(e);
    e->mustRemove = true;
}
