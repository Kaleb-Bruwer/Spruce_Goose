#ifndef OCTINTERNAL_H
#define OCTINTERNAL_H

#include "OctNode.h"

#include <gtest/gtest.h>

using namespace std;

class OctInternal : public OctNode{
private:
    //OctRoot has its own implementation
    //Not worth making virtual since they are always called from this class
    static int index(bool x, bool y, bool z);
    int index(Coordinate<double> pos); //-1 if out of bounds

protected:
    OctNode* children[8];
    Coordinate<double> midpoint;

    static int marginFloor(double);

    //Used by OctRoot, which inherits from this class
    OctInternal();
public:

    OctInternal(Coordinate<double> lc, Coordinate<double> hc, OctNode* parent);
    virtual ~OctInternal(){};

    virtual OctNode* insert(Positional* p, bool flag);

    // ASSUMED WITHIN BOUNDS
    virtual OctNode* remove(Positional* p);
    void execFunc(Functor<Positional*> &f, bool flag);
    double execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f, bool flag);
    pair<Coordinate<double>, Coordinate<double>>
        execInBounds(Coordinate<double> lc, Coordinate<double> hc, Functor<Positional*> &f, bool flag);

    void resetFlagsInRadius(Coordinate<double> pos, double r, bool val);
    void resetFlagsInBounds(Coordinate<double> lc, Coordinate<double> hc, bool val);

    void deepDelete();

    //Only to be used on new nodes
    // returns true if Pos has to move nodes
    void addChild(OctNode* c);

    virtual bool fixPos(Positional* p, Coordinate<double> oldPos, bool flag);

    // Tests with private access
    FRIEND_TEST(OctInternalTest, insert1);
    FRIEND_TEST(OctInternalInsertTest, insert);
    FRIEND_TEST(OctInternalTest, remove1);
    FRIEND_TEST(OctInternalTest, remove2);
    FRIEND_TEST(OctInternalTest, remove3);
    FRIEND_TEST(OctInternalTest, exec1);

};

#endif
