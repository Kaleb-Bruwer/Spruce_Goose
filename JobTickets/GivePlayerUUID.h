#ifndef GIVEPLAYERUUID_H
#define GIVEPLAYERUUID_H

#include "JobTicket.h"

using namespace std;

class GivePlayerUUID : public JobTicket{
public:
    string uuid;
    GivePlayerUUID(string u){
        uuid = u;
    }
    string getType(){return "GivePlayerUUID";}
};

#endif
