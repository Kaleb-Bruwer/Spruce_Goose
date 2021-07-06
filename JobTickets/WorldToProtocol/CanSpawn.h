#pragma once

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class CanSpawn : public JobTicket{
public:
    Coordinate<double> spawnPos;
    float yaw;
    float pitch;

    CanSpawn(){};
    CanSpawn(Coordinate<double> p, float y, float pi){
        spawnPos = p;
        yaw = y;
        pitch = pi;
    };

    string getType(){return "CanSpawn";};
};
