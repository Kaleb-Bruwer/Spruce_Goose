#ifndef TIEREDTILE_H
#define TIEREDTILE_H

#include <vector>

#include "TilePos.h"

using namespace std;

//Has TilePos indirectly associated with it via its position in TieredGrid

template <class T>
class TieredTile{
public:
    vector<T> elements;

    TieredTile(){};
};


#endif
