#ifndef TIEREDGRID_H
#define TIEREDGRID_H

#include <map>
#include <vector>

#include "TieredTile.h"
#include "TilePos.h"

#include "../../General/Functor.h"

using namespace std;

template <class T>
class TieredGrid{
public:
    map<TilePos, TieredTile<T>*> grid;

    TieredGrid(){};
    ~TieredGrid();

    void insert(TilePos, T);
    TieredTile<T>* getTile(TilePos);
    vector<TieredTile<T>*> getAllTiles();

    void executeFunctor(Functor<T> &f);
    
};

#endif
