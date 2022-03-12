#ifndef DESTROYENTITYJOB_H
#define DESTROYENTITYJOB_H

#include <climits>

#include "../JobTicket.h"

using namespace std;

class DestroyEntityJob : public JobTicket{
public:
    DestroyEntityJob() : JobTicket(DESTROYENTITYJOB){};
    DestroyEntityJob(DestroyEntityJob &j) : JobTicket(DESTROYENTITYJOB){
        eid = j.eid;
    };

    int eid;
    int pickupBy = INT_MAX;
};

#endif
