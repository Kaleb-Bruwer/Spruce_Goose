#include "TieredGrid.h"

using namespace std;

template <class T>
TieredGrid<T>::~TieredGrid(){
    for(auto it = grid.begin(); it != grid.end(); it++){
        delete it->second;
    }
}

template <class T>
void TieredGrid<T>::insert(TilePos pos, T elem){
    auto it = grid.find(pos);
    if(it == grid.end()){
        grid.emplace(pos, new TieredTile<T>());
    }
    grid[pos]->elements.push_back(elem);
}

template <class T>
TieredTile<T>* TieredGrid<T>::getTile(TilePos pos){
    auto it = grid.find(pos);
    if(it != grid.end())
        return it->second;
    return 0;
}

template <class T>
vector<TieredTile<T>*> TieredGrid<T>::getAllTiles(){
    vector<TieredTile<T>*> result;

    for(auto it = grid.begin(); it != grid.end(); it++){
        result.push_back(it->second);
    }

    return result;
}

template <class T>
void TieredGrid<T>::executeFunctor(Functor<T> &f){
    for(auto tileIT = grid.begin(); tileIT != grid.end(); tileIT++){
        for(T e : tileIT->second->elements){
            f(e);
        }
    }
}

class Entity;

template class TieredGrid<Entity*>;
