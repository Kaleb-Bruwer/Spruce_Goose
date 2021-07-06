#ifndef GIVEPLAYERUUID_H
#define GIVEPLAYERUUID_H

#include "JobTicket.h"

using namespace std;

class GivePlayerUUID : public JobTicket{
public:
    string uuid;
    GivePlayerUUID(string u) : JobTicket(GIVEPLAYERUUID){
        uuid = u;
    }
};

#endif
