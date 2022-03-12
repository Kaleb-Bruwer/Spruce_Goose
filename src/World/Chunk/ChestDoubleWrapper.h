#pragma once

#include "../../Datastructures/Coordinate/Coordinate.h"

class ChestDouble;

class ChestDoubleWrapper{
public:
    ChestDouble* chest = 0;
    // pos1 is first half , pos2 second half
    Coordinate<int> pos1;
    Coordinate<int> pos2;

    ~ChestDoubleWrapper();

    ChestDoubleWrapper* clone();
};
