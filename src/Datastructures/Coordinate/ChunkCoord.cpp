#include "ChunkCoord.h"

#include <cmath>

#include "RegionCoord.h"

using namespace std;

ChunkCoord::ChunkCoord(int xIn, int zIn){
    x = xIn;
    z = zIn;
}

ChunkCoord::ChunkCoord(ChunkCoord* c){
    x = c->x;
    z = c->z;
}

RegionCoord ChunkCoord::getRegion(){
    RegionCoord result;
    result.x = floor((double)x/32);
    result.z = floor((double)z/32);

    return result;
}

bool ChunkCoord::compare(const ChunkCoord& lhs, const ChunkCoord& rhs) const{
    if(lhs.x < rhs.x)
        return true;
    if(lhs.x > rhs.x)
        return false;

    if(lhs.z < rhs.z)
        return true;
    return false;
}

bool ChunkCoord::operator<(const ChunkCoord &rhs) const{
    return compare(*this, rhs);
}

bool ChunkCoord::operator>(const ChunkCoord &rhs) const{
    return compare(rhs, *this);
}

bool ChunkCoord::operator==(const ChunkCoord &rhs) const{
    return (this->x == rhs.x
        && this->z == rhs.z);
}

bool ChunkCoord::operator!=(const ChunkCoord &rhs) const{
    return !(this->x == rhs.x
        && this->z == rhs.z);
}

void ChunkCoord::movNegX(){
    x--;
}
void ChunkCoord::movPosX(){
    x++;
}
void ChunkCoord::movNegZ(){
    z--;
}
void ChunkCoord::movPosZ(){
    z++;
}
