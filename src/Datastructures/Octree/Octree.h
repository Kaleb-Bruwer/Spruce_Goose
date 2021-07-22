#ifndef OCTREE_H
#define OCTREE_H

#include "OctNode.h"
#include "Positional.h"

using namespace std;

class Octree{
private:
    const double baseWidth = 256;

    OctNode* root = 0;
    bool currFlag = false;

public:
    Octree(){};
    ~Octree();

    void insert(Positional* val);
    void remove(Positional* val); //Doesn't delete val, just removes it
    void execFunc(Functor<Positional*> &f);
    void execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f);
    void execInBounds(Coordinate<double> lc, Coordinate<double> rc, Functor<Positional*> &f);

    //For when an element's position is changed from outside
    void fixPos(Positional* p, Coordinate<double> oldPos);
};

#endif
