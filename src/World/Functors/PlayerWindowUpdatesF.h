#ifndef PLAYERWINDOWUPDATESF_H
#define PLAYERWINDOWUPDATESF_H

#include "../../General/Functor.h"

using namespace std;

class PlayerEntity;

class PlayerWindowUpdatesF : public Functor<PlayerEntity*>{
public:
    void operator()(PlayerEntity* p);
};

#endif
