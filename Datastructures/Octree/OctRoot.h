#ifndef OCTROOT_H
#define OCTROOT_H

#include "OctInternal.h"

using namespace std;

class OctRoot : public OctInternal{
private:
    //midpoint is (0,0,0)

    int index(Coordinate<double> pos);

    //pos is used as a reference to make child with ideal bounds
    void makeChild(int i, Coordinate<double> pos);
    void expandChild(int i);

    bool isInside(Coordinate<double> pos){return true;};
    bool withinBounds(Coordinate<double> lc, Coordinate<double> hc){return true;};
public:
    OctRoot(){};
    ~OctRoot(){};

    OctNode* insert(Positional* val, bool flag);
    OctNode* remove(Positional* val);

    bool fixPos(Positional* p, Coordinate<double> oldPos, bool flag);

};

#endif
