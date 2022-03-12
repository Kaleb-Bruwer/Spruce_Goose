#ifndef OCTNODE_H
#define OCTNODE_H

#include <cmath>
#include <utility>

#include "Positional.h"
#include "../Coordinate/Coordinate.h"
#include "../../General/Functor.h"

using namespace std;

class OctRoot;

class OctNode{
public:
    //Point at which a leaf node gets split
    //This is not in OctLeaf so that OctInternal can also know it
    const int threshold = 12;

    Coordinate<double> lowCorner;
    Coordinate<double> highCorner;
    OctNode* parent = 0;

    virtual bool isInside(Coordinate<double> pos);

    // Returns true if there is ANY overlap
    virtual bool withinBounds(Coordinate<double> lc, Coordinate<double> hc);

    friend class OctRoot;

    OctNode(){};

public:
    OctNode(Coordinate<double> lc, Coordinate<double> hc);

    //recursively deleting has its own method, so that nodes can more easily
    //be deleted when the tree is just being altered
    virtual ~OctNode(){};

    double distFrom(Coordinate<double> pos); //Returns 0 if inside

    virtual OctNode* insert(Positional* val, bool flag) = 0;
    virtual OctNode* remove(Positional* val) = 0;
    virtual void execFunc(Functor<Positional*> &f, bool flag) = 0;

    //Return value is needed to determine bounds within which flags must be reset
    virtual double execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f, bool flag) = 0;
    virtual pair<Coordinate<double>, Coordinate<double>>
        execInBounds(Coordinate<double> lc, Coordinate<double> hc, Functor<Positional*> &f, bool flag) = 0;

    virtual void resetFlagsInRadius(Coordinate<double> pos, double r, bool val) = 0;
    virtual void resetFlagsInBounds(Coordinate<double> lc, Coordinate<double> hc, bool val) = 0;

    virtual void deepDelete() = 0;//Deletes everything down to the Positionals
    Coordinate<double> getLowCorner(){return lowCorner;};

    virtual bool fixPos(Positional* p, Coordinate<double> oldPos, bool flag) = 0;
};

#endif
