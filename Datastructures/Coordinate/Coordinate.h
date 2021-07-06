#ifndef COORDINATE_H
#define COORDINATE_H

#include <iostream>

#include "ChunkCoord.h"

using namespace std;

class RegionCoord;

template <class T>
class Coordinate{
public:
    T x=0, y=0, z=0;

    Coordinate(){};
    Coordinate(T x, T y, T z);
    Coordinate(Coordinate *c);

    //These methods can be called on velocities, but shouldn't be
    void insideChunk();//Not reversible
    void insideChunkSection();

    ChunkCoord getContainingChunk();
    RegionCoord getContainingRegion();

    //To be called on velocities to get speed
    double distFromOrigin();

    Coordinate& operator+=(Coordinate& rhs);
    Coordinate<double> operator/(double val);
    Coordinate<double> getLinearDiv(double val); //Result in new vector
    double getDist(Coordinate<T>& );

    bool operator<(const Coordinate<T>& rhs) const;
    bool operator>(const Coordinate<T>& rhs) const;
    bool operator!=(const Coordinate<T> &rhs);
    bool operator==(const Coordinate<T> &rhs);
    Coordinate<T> operator+(const Coordinate<T> &rhs);
    Coordinate<T> operator-(const Coordinate<T> &rhs);

    friend ostream& operator<< (ostream &out, const Coordinate<T> &rhs){
        out << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
    };

};

#endif
