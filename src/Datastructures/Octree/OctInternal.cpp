#include "OctInternal.h"

#include <cstring>
#include <iostream>

#include "OctLeaf.h"

using namespace std;

OctInternal::OctInternal(){
    memset(children, 0, sizeof(children));
}

OctInternal::OctInternal(Coordinate<double> lc, Coordinate<double> hc, OctNode* p) : OctNode(lc, hc){
    memset(children, 0, sizeof(children));
    midpoint.x = (lc.x + hc.x)/2;
    midpoint.y = (lc.y + hc.y)/2;
    midpoint.z = (lc.z + hc.z)/2;

    parent = p;
}

int OctInternal::index(bool x, bool y, bool z){
    int index = 0;
    if(z)
        index += 4;
    if(y)
        index += 2;
    if(x)
        index += 1;
    return index;
}

int OctInternal::marginFloor(double val){
    int base = floor(val);

    //The val > base check ensures that this increase is only applied
    //when the normal floor failed (It does nothing if floor(1) == 1)
    //It is simply meant to fix floor(0.9999) = 0
    if(val > base && (val - base) > 0.95)
        base++;
    return base;
}

int OctInternal::index(Coordinate<double> p){
    if(p.x < lowCorner.x
            || p.x >= highCorner.x
            || p.y < lowCorner.y
            || p.y >= highCorner.y
            || p.z < lowCorner.z
            || p.z >= highCorner.z)
        return -1;

    int i = 0;
    if(p.x >= midpoint.x)
        i += 1;
    if(p.y >= midpoint.y)
        i += 2;
    if(p.z >= midpoint.z)
        i += 4;

    return i;
}

OctNode* OctInternal::insert(Positional* p, bool flag){
    bool xS = 0, yS = 0, zS = 0;

    int i = index(p->position);

    if(i == -1){
        parent->insert(p, flag);
        //In this scenario the return value doesn't matter since if the
        //insert came from above, this code would never be reached
        return this;
    }

    if(!children[i]){
        Coordinate<double> lc;
        Coordinate<double> hc;
        if(i & 1){
            lc.x = midpoint.x;
            hc.x = highCorner.x;
        }
        else{
            lc.x = lowCorner.x;
            hc.x = midpoint.x;
        }

        if(i & 2){
            lc.y = midpoint.y;
            hc.y = highCorner.y;
        }
        else{
            lc.y = lowCorner.y;
            hc.y = midpoint.y;
        }

        if(i & 4){
            lc.z = midpoint.z;
            hc.z = highCorner.z;
        }
        else{
            lc.z = lowCorner.z;
            hc.z = midpoint.z;
        }
        children[i] = new OctLeaf(lc, hc, this);
    }
    children[i] = children[i]->insert(p, flag);
    return this;
}

OctNode* OctInternal::remove(Positional* val){
    int cIndex = index(val->position);

    children[cIndex] = children[cIndex]->remove(val);
    if(children[cIndex])
        return this;
    //If child has been deleted, check how many are left
    //and delete this node if it no longer has any children
    for(int i=0; i<8; i++){
        if(children[i])
            return this;
    }

    //If empty, delete this node
    delete this;
    return 0;
}

void OctInternal::execFunc(Functor<Positional*> &f, bool flag){
    for(int i=0; i<8; i++){
        if(children[i])
            children[i]->execFunc(f, flag);
    }
}

double OctInternal::execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f, bool flag){
    double maxDeviation = 0; //only stays below r if functor never executed
    for(int i=0; i<8; i++){
        if(children[i] && children[i]->distFrom(pos) <= r)
            maxDeviation = max(children[i]->execFuncInRadius(pos, r, f, flag), maxDeviation);
    }
    return maxDeviation;
}

pair<Coordinate<double>, Coordinate<double>>
OctInternal::execInBounds(Coordinate<double> lc, Coordinate<double> hc, Functor<Positional*> &f, bool flag){
    Coordinate<double> retLow = lc;
    Coordinate<double> retHigh = hc;

    for(int i=0; i<8; i++){
        if(!children[i])
            continue;

        Coordinate<double> low = children[i]->lowCorner;
        Coordinate<double> high = children[i]->highCorner;

        // Check for no bounding box overlap
        if( lc.x >= high.x ||
            lc.y >= high.y ||
            lc.z >= high.z ||
            hc.x < low.x ||
            hc.y < low.y ||
            hc.z < low.z)
            continue;


        pair<Coordinate<double>, Coordinate<double>> result;
        result = children[i]->execInBounds(lc, hc, f, flag);

        Coordinate<double> l = result.first;
        Coordinate<double> h = result.second;
        retLow.x = min(l.x, retLow.x);
        retLow.y = min(l.y, retLow.y);
        retLow.z = min(l.z, retLow.z);

        retHigh.x = max(h.x, retHigh.x);
        retHigh.y = max(h.y, retHigh.y);
        retHigh.z = max(h.z, retHigh.z);
    }

    return make_pair(retLow, retHigh);
}


void OctInternal::resetFlagsInRadius(Coordinate<double> pos, double r, bool val){
    for(int i=0; i<8; i++){
        if(children[i] && children[i]->distFrom(pos) <= r)
            children[i]->resetFlagsInRadius(pos, r, val);
    }
}

void OctInternal::resetFlagsInBounds(Coordinate<double> lc, Coordinate<double> hc, bool val){
    if(!withinBounds(lc, hc))
        return;
    for(int i=0; i<8; i++){
        if(children[i])
            children[i]->resetFlagsInBounds(lc, hc, val);
    }
}

void OctInternal::deepDelete(){
    for(int i=0; i<8; i++){
        if(children[i]){
            children[i]->deepDelete();
            delete children[i];
        }
    }
}

void OctInternal::addChild(OctNode* c){
    int i = index(c->getLowCorner());
    if(i == -1){
        cout << "Something went really wrong at OctInternal::addChild\n";
        return;
    }

    children[i] = c;
}

bool OctInternal::fixPos(Positional* p, Coordinate<double> oldPos, bool flag){
    int i = index(oldPos);

    if(!children[i]){
        cout << "OctInternal::fixPos called wrong\n";
        return false;
    }

    if(children[i]->fixPos(p, oldPos, flag)){
        insert(p, flag);
    }
}
