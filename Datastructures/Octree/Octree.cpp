#include "Octree.h"

#include <cmath>
#include <iostream>

#include "OctLeaf.h"
#include  "OctInternal.h"
#include  "OctRoot.h"

Octree::~Octree(){
    if(root){
        root->deepDelete();
    }
    delete root;
}

void Octree::insert(Positional* val){
    if(!root){
        //The point of this is that
        Coordinate<double> lc = val->position;
        lc.x = floor(lc.x/baseWidth) * baseWidth;
        lc.y = floor(lc.y/baseWidth) * baseWidth;
        lc.z = floor(lc.z/baseWidth) * baseWidth;

        Coordinate<double> hc = lc;
        hc.x += baseWidth;
        hc.y += baseWidth;
        hc.z += baseWidth;

        root = new OctRoot();
    }
    root = root->insert(val, currFlag);

}

void Octree::remove(Positional* val){
    root = root->remove(val);
}

void Octree::execFunc(Functor<Positional*> &f){
    //The purpose of the flag is to only execute on each element once
    root->execFunc(f, currFlag);
    currFlag = !currFlag;
}

void Octree::execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f){
    double dist = root->execFuncInRadius(pos, r, f, currFlag);
    root->resetFlagsInRadius(pos, dist, currFlag);
}

void Octree::execInBounds(Coordinate<double> lc, Coordinate<double> rc, Functor<Positional*> &f){
    root->execInBounds(lc, rc, f, currFlag);
}

void Octree::fixPos(Positional* p, Coordinate<double> oldPos){
    root->fixPos(p, oldPos, currFlag);
}
