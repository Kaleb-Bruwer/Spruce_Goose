#pragma once

#include "../JobTicket.h"

class EntityActionJob : public JobTicket{
public:
    EntityActionJob() : JobTicket(ENTITYACTIONJOB){};

    int eid;
    char actionID;
    int jumpBoost; //For horses

};
