#ifndef DESTROYENTITYJOB_H
#define DESTROYENTITYJOB_H

#include <climits>

#include "../JobTicket.h"

using namespace std;

class DestroyEntityJob : public JobTicket{
public:
    DestroyEntityJob(){};
    DestroyEntityJob(DestroyEntityJob &j){
        eid = j.eid;
    };

    int eid;
    int pickupBy = INT_MAX;

    string getType(){return "DestroyEntityJob";};
};

#endif
