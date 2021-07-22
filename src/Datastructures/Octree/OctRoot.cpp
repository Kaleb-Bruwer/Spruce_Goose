#include "OctRoot.h"

#include <iostream>

using namespace std;

int OctRoot::index(Coordinate<double> p){
    int i = 0;
    if(p.x >= 0)
        i += 1;
    if(p.y >= 0)
        i += 2;
    if(p.z >= 0)
        i += 4;

    /*
    Add this, but test it first
    i = (p.x >= 0) + (p.y>=0)*2 + (p.z>=0)*4;

    */

    return i;
}

void OctRoot::makeChild(int i, Coordinate<double> pos){
    if(children[i])
        return;

    Coordinate<double> lc;
    Coordinate<double> hc;

    lc.x = floor(pos.x/256) * 256;
    lc.y = floor(pos.y/256) * 256;
    lc.z = floor(pos.z/256) * 256;

    hc.x = lc.x + 256;
    hc.y = lc.y + 256;
    hc.z = lc.z + 256;

    children[i] = new OctInternal(lc, hc, this);
}

void OctRoot::expandChild(int i){
    Coordinate<double> lc;
    Coordinate<double> hc;
    Coordinate<double> lowCorner = children[i]->lowCorner;
    Coordinate<double> highCorner = children[i]->highCorner;

    double w = (highCorner.x - lowCorner.x)*2;

    lc.x = marginFloor(lowCorner.x/w) * w;
    lc.y = marginFloor(lowCorner.y/w) * w;
    lc.z = marginFloor(lowCorner.z/w) * w;

    hc.x = lc.x + w;
    hc.y = lc.y + w;
    hc.z = lc.z + w;

    //OctInternal(lowCorner, highCorner, parent)
    OctInternal* rep = new OctInternal(lc, hc, this);
    children[i]->parent = rep;
    rep->addChild(children[i]);
    children[i] = rep;

    lowCorner = children[i]->lowCorner;
    highCorner = children[i]->highCorner;
}

OctNode* OctRoot::insert(Positional* val, bool flag){
    int i = index(val->position);
    if(!children[i]){
        makeChild(i, val->position);
    }

    while(!children[i]->isInside(val->position)){
        //Make new node with wider bounds
        expandChild(i);
    }
    OctNode* temp = children[i];

    children[i] = children[i]->insert(val, flag);
    return this;

}

OctNode* OctRoot::remove(Positional* val){
    int i = index(val->position);

    if(children[i])
        children[i] = children[i]->remove(val);
}

bool OctRoot::fixPos(Positional* p, Coordinate<double> oldPos, bool flag){
    int i = index(oldPos);

    if(!children[i]){
        //Shouldn't happen
        cout << "OctRoot::fixPos called wrong\n";
        return false;
    }

    if(children[i]->fixPos(p, oldPos, flag)){
        insert(p, flag);
    }

    return false;
}
