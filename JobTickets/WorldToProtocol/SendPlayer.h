#ifndef SENDPLAYER_H
#define SENDPLAYER_H

#include "../JobTicket.h"

using namespace std;

class SendPlayer : public JobTicket{
public:
    int eid;
    string username;
    Coordinate<double> pos;
    float pitch;
    float yaw;
    short item = 0; //id of held item, 0 means none
    

    string getType(){return "SendPlayer";};
};

#endif
