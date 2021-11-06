#pragma once

#include "../../Datastructures/Coordinate/Coordinate.h"

class ChestDouble;

class ChestDoubleWrapper{
public:
    ChestDouble* chest = 0;
    Coordinate<int> pos1;
    Coordinate<int> pos2;

    ~ChestDoubleWrapper();
};
