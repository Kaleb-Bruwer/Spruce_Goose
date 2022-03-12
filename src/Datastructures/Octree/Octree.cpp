#include "Octree.h"

#include <cmath>
#include <iostream>

#include "OctLeaf.h"
#include  "OctInternal.h"

Octree::Octree(){}

Octree::~Octree(){}

void Octree::insert(Positional* val){
    root.insert(val, currFlag);
}

void Octree::remove(Positional* val){
    root.remove(val);
}

void Octree::execFunc(Functor<Positional*> &f){
    //The purpose of the flag is to only execute on each element once
    root.execFunc(f, currFlag);
    currFlag = !currFlag;
}

void Octree::execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f){
    double dist = root.execFuncInRadius(pos, r, f, currFlag);
    root.resetFlagsInRadius(pos, dist, currFlag);
}

void Octree::execInBounds(Coordinate<double> lc, Coordinate<double> rc, Functor<Positional*> &f){
    pair<Coordinate<double>, Coordinate<double>> result;
    result = root.execInBounds(lc, rc, f, currFlag);

    root.resetFlagsInBounds(result.first, result.second, currFlag);
}

void Octree::fixPos(Positional* p, Coordinate<double> oldPos){
    root.fixPos(p, oldPos, currFlag);
}
