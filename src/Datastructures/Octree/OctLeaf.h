#ifndef OCTLEAF_H
#define OCTLEAF_H

#include "OctNode.h"

#include <vector>
#include <utility>

#include "Positional.h"

using namespace std;

class OctLeaf : public OctNode{
private:
    vector<pair<Positional*, bool>> elements;

    //Return value is node that now stands in this one's place
    //This method also deletes its object, it must be the last thing you call
    OctNode* split();
public:

    OctLeaf(Coordinate<double> lc, Coordinate<double> hc, OctNode* p);
    virtual ~OctLeaf(){};

    OctNode* insert(Positional* val, bool flag);
    OctNode* remove(Positional* val);
    void execFunc(Functor<Positional*> &f, bool flag);
    double execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f, bool flag);
    pair<Coordinate<double>, Coordinate<double>>
        execInBounds(Coordinate<double> lc, Coordinate<double> hc, Functor<Positional*> &f, bool flag);

    void resetFlagsInRadius(Coordinate<double> pos, double r, bool val);
    void resetFlagsInBounds(Coordinate<double> lc, Coordinate<double> hc, bool val);

    void deepDelete();

    // Removes p if oldPos is outside this node
    // TODO: figure out what this is for
    bool fixPos(Positional* p, Coordinate<double> oldPos, bool flag);


};

#endif
