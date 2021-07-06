#include "TilePos.h"

#include <cmath>


using namespace std;

TilePos::TilePos(int xIn, int yIn, int zIn, int lIn){
    x = xIn;
    y = yIn;
    z = zIn;
    l = lIn;
}


TilePos& TilePos::operator=(const TilePos& rhs){
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    l = rhs.l;
}

bool TilePos::compare(const TilePos& lhs, const TilePos& rhs) const{
    if(lhs.l < rhs.l)
        return true;
    if(lhs.l > rhs.l)
        return false;

    if(lhs.x < rhs.x)
        return true;
    if(lhs.x > rhs.x)
        return false;

    if(lhs.y < rhs.y)
        return true;
    if(lhs.y > rhs.y)
        return false;

    if(lhs.z < rhs.z)
        return true;
    return false;
}

bool TilePos::operator<(const TilePos& rhs) const{
    return compare(*this, rhs);
}

bool TilePos::operator>(const TilePos& rhs) const{
    return compare(rhs, *this);
}

bool TilePos::operator==(const TilePos& rhs) const{
    return (this->l == rhs.l
        && this->x == rhs.x
        && this->y == rhs.y
        && this->z == rhs.z);
}

bool TilePos::operator!=(const TilePos& rhs) const{
    return !operator==(rhs);
}

void TilePos::beParent(){
    x = floor((float)x/2);
    y = floor((float)y/2);
    z = floor((float)z/2);
    l++;
}

void TilePos::beChild(bool x1, bool y1, bool z1){
    l--;
    x *= 2;
    y *= 2;
    z *= 2;

    if(x1)
        x++;
    if(y1)
        y++;
    if(z1)
        z++;
}
