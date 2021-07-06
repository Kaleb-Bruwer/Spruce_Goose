#ifndef POSITIONAL_H
#define POSITIONAL_H

using namespace std;

#include "../Coordinate/Coordinate.h"

//Any class that will be stored in an Octree must publicly inherit this class
//It is needed to update the octree as functors are executed on elements
class Positional{
public:
    virtual ~Positional(){};

    Coordinate<double> position;

    //Will be removed if either of these are true
    //but will only be deleted if mustDelete is true
    //mustRemove will be reset if not deleted, just
    //Don't touch this object with another functor until mustRemove has reset
    bool mustDelete = false;
    bool mustRemove = false;
};

#endif
