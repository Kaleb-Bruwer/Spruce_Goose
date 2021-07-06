#ifndef REGIONCOORD_H
#define REGIONCOORD_H

#include <iostream>

using namespace std;

//This is a separate class from ChunkCoord so that they can't get mixed up,
//it basically enforces a type system

class RegionCoord{
private:
    bool compare(const RegionCoord& lhs, const RegionCoord& rhs) const;
public:
    int x=0, z=0;
    RegionCoord(){};
    RegionCoord(int x, int z);
    RegionCoord(RegionCoord*);

    //xDiff + zDiff
    int cardinalDist(const RegionCoord &rhs);

    bool operator<(const RegionCoord& rhs) const;
    bool operator>(const RegionCoord& rhs) const;
    bool operator==(const RegionCoord& rhs) const;
    bool operator!=(const RegionCoord& rhs) const;

    friend ostream& operator<< (ostream &out, const RegionCoord &rhs){
        out << "(" << rhs.x << ", " << rhs.z << ")";
    };

    void movNegX();
    void movPosX();
    void movNegZ();
    void movPosZ();
};

#endif
