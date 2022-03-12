#include "RegionCoord.h"

#include <cmath>

using namespace std;

RegionCoord::RegionCoord(int xIn, int zIn){
    x = xIn;
    z = zIn;
}

RegionCoord::RegionCoord(RegionCoord* c){
    x = c->x;
    z = c->z;
}

int RegionCoord::cardinalDist(const RegionCoord &rhs){
    return abs(x - rhs.x) + abs(z - rhs.z);
}

bool RegionCoord::compare(const RegionCoord& lhs, const RegionCoord& rhs) const{
    if(lhs.x < rhs.x)
        return true;
    if(lhs.x > rhs.x)
        return false;

    if(lhs.z < rhs.z)
        return true;
    return false;
}

bool RegionCoord::operator<(const RegionCoord &rhs) const{
    return compare(*this, rhs);
}

bool RegionCoord::operator>(const RegionCoord &rhs) const{
    return compare(rhs, *this);
}

bool RegionCoord::operator==(const RegionCoord &rhs) const{
    return (this->x == rhs.x
        && this->z == rhs.z);
}

bool RegionCoord::operator!=(const RegionCoord &rhs) const{
    return !(this->x == rhs.x
        && this->z == rhs.z);
}

void RegionCoord::movNegX(){
    x--;
}
void RegionCoord::movPosX(){
    x++;
}
void RegionCoord::movNegZ(){
    z--;
}
void RegionCoord::movPosZ(){
    z++;
}
