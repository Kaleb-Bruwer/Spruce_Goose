#ifndef CHUNKCOORD_H
#define CHUNKCOORD_H

#include <iostream>

using namespace std;

class RegionCoord;

class ChunkCoord{
private:
    bool compare(const ChunkCoord& lhs, const ChunkCoord& rhs) const;
public:
    int x=0, z=0;
    ChunkCoord(){};
    ChunkCoord(int x, int z);
    ChunkCoord(ChunkCoord*);

    RegionCoord getRegion();

    bool operator<(const ChunkCoord& rhs) const;
    bool operator>(const ChunkCoord& rhs) const;
    bool operator==(const ChunkCoord& rhs) const;
    bool operator!=(const ChunkCoord& rhs) const;
    ChunkCoord& operator+=(const ChunkCoord &rhs);

    friend ostream& operator<< (ostream &out, const ChunkCoord &rhs){
        out << "(" << rhs.x << ", " << rhs.z << ")";
    };


    void movNegX();
    void movPosX();
    void movNegZ();
    void movPosZ();
};

#endif
