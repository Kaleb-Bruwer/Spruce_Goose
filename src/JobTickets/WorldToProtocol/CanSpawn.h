#pragma once

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class CanSpawn : public JobTicket{
public:
    Coordinate<double> spawnPos;
    float yaw;
    float pitch;

    CanSpawn() : JobTicket(CANSPAWN){};
    CanSpawn(Coordinate<double> p, float y, float pi) : JobTicket(CANSPAWN){
        spawnPos = p;
        yaw = y;
        pitch = pi;
    };
};
