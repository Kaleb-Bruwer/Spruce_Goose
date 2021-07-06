#ifndef SENDPLAYERLOGIN_H
#define SENDPLAYERLOGIN_H

#include "../JobTicket.h"

#include <iostream>

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class PlayerEntity;

class SendPlayerLogin : public JobTicket{
public:
    Coordinate<double> pos;
    Coordinate<int> spawnPos;
    double pitch = 0;
    double yaw = 0;
    int eid;
    int renderDistance = 3;

    SendPlayerLogin(PlayerEntity*);

    SendPlayerLogin(Coordinate<double> p, int e, double pi = 0, double y = 0){
        pos = p;
        eid = e;
        pitch = pi;
        yaw = y;
    }

    string getType(){
        return "SendPlayerLogin";
    }
};

#endif
