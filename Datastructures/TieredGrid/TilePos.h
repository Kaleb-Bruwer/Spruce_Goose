#ifndef TILEPOS_H
#define TILEPOS_H

#include <cmath>

#include "../Coordinate/Coordinate.h"

using namespace std;

class TilePos{
private:
    bool compare(const TilePos& lhs,const TilePos& rhs) const;
public:
    int x=0, y=0, z=0, l=0; //l is layer, usually velocity

    TilePos(){};
    TilePos(int xIn, int yIn, int zIn, int lIn = 0);

    TilePos& operator=(const TilePos& rhs);
    bool operator<(const TilePos& rhs) const;
    bool operator>(const TilePos& rhs) const;
    bool operator==(const TilePos& rhs) const;
    bool operator!=(const TilePos& rhs) const;

    void beParent();
    void beChild(bool x, bool y, bool z);//booleans are offsets
};

/*
inline bool operator<(const TilePos& lhs, const TilePos& rhs){
    return lhs.operator<(rhs);
}

inline bool operator>(const TilePos& lhs, const TilePos& rhs){
    return lhs.operator>(rhs);
}

inline bool operator==(const TilePos& lhs, const TilePos& rhs){
    return lhs.operator==(rhs);
}

inline bool operator!=(const TilePos& lhs, const TilePos& rhs){
    return lhs.operator!=(rhs);
}
*/

#endif
