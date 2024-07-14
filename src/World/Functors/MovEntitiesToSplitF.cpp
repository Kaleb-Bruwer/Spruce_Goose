#include "MovEntitiesToSplitF.h"

#include <iostream>

#include "../ThreadArea.h"

using namespace std;

MovEntitiesToSplitF::MovEntitiesToSplitF(ThreadArea* d){
    dest = d;
}

void MovEntitiesToSplitF::operator()(Entity* e){
    dest->addEntity(e);
    // mustRemove will be set to false again before dest gets its thread, so this is safe
    // This also protects entities from being destroyed in case Entity gets a destructor
    e->mustRemove = true;
}
