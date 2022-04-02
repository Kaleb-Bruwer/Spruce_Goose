#include "ChunkMap.h"

#include <algorithm>
#include <iostream>

using namespace std;

template <class T>
ChunkMap<T>::ChunkMap(){}

template <class T>
ChunkMap<T>::~ChunkMap(){}

template <class T>
T ChunkMap<T>::getVal(ChunkCoord c) const{
    auto it = values.find(c);
    if(it == values.end())
        return 0;
    return it->second;
}

template <class T>
T ChunkMap<T>::getVal(Coordinate<int> c) const{
    return getVal(c.getContainingChunk());
}

template <class T>
T ChunkMap<T>::getVal(Coordinate<double> c) const{
    return getVal(c.getContainingChunk());
}

template <class T>
void ChunkMap<T>::setVal(ChunkCoord c, T val){
    if(val != 0){
        values[c] = val;
    }
    else{
        values.erase(c);
    }
}

template <class T>
vector<ChunkCoord> ChunkMap<T>::getRing(ChunkCoord center, int radius){
    //Probably intended for chunkloading with a priority system
    vector<ChunkCoord> result;

    ChunkCoord temp = center;
    temp.x -= radius;
    temp.z -= radius;
    //temp is not top left corner

    for(int i=0; i<radius*2; i++){
        result.push_back(temp);
        temp.x++;
    }
    for(int i=0; i<radius*2; i++){
        result.push_back(temp);
        temp.z++;
    }
    for(int i=0; i<radius*2; i++){
        result.push_back(temp);
        temp.x--;
    }
    for(int i=0; i<radius*2; i++){
        result.push_back(temp);
        temp.z--;
    }
    return result;
}

template <class T>
vector<T> ChunkMap<T>::getRingValues(ChunkCoord center, int radius){
    //Probably intended for chunkloading with a priority system
    vector<T> result;

    vector<ChunkCoord> positions = getRing(center, radius);
    for(ChunkCoord pos : positions){
        T val = getVal(pos);
        if(val == 0)
            continue;

        auto it = find(result.begin(), result.end(), val);
        if(it == result.end())
            result.push_back(val);
    }

    return result;
}

template <class T>
T ChunkMap<T>::getClosest(ChunkCoord center, int maxDist){
    //Not efficient, improve later by changing internal datastructure
    if(values.size() == 0)
        return 0;

    auto it = values.find(center);
    if(it != values.end())
        return it->second;


    for(int r=0; r<maxDist; r++){
        vector<T> ring = getRingValues(center, r);
        if(ring.size() > 0)
            return ring[0];
    }

    //If this part is reached, there's no threadArea nearby
    return 0;
}

template <class T>
vector<T> ChunkMap<T>::getAllNearby(ChunkCoord c, int radius){
    const int defaultRadius = 12;
    if(radius == 0){
        radius = defaultRadius;
    }

    vector<T> result;

    ChunkCoord base = c;
    base.x -= radius;
    base.z -= radius;
    ChunkCoord iterator = base;

    for(int w=0; w<radius*2; w++){
        T temp = getVal(iterator);

        if(temp){
            bool duplicate = false;
            for(T temp2: result){
                if(temp == temp2){
                    duplicate = true;
                    break;
                }
            }

            if(!duplicate){
                result.push_back(temp);
            }
        }

    }
    return result;
}

template <class T>
vector<T> ChunkMap<T>::getAll(){
    vector<T> result;

    for(auto it = values.begin(); it != values.end(); it++){
        auto it2 = find(result.begin(), result.end(), it->second);
        if(it2 == result.end())
            result.push_back(it->second);
    }

    return result;
}

template <class T>
vector<ChunkCoord> ChunkMap<T>::getAllPositions(){
    vector<ChunkCoord> result;

    for(auto it = values.begin(); it != values.end(); it++){
        if(it->second != 0)
            result.push_back(it->first);
    }

    return result;
}

template <class T>
void ChunkMap<T>::executeFunctor(Functor<T> &f){
    for(auto it = values.begin(); it != values.end(); it++){
        if(it->second != 0)
            f(it->second);
    }
}

template <class T>
void ChunkMap<T>::copyTo(ChunkMap<T>* dest){
    for(auto it = values.begin(); it != values.end(); it++){
        dest->setVal(it->first, it->second);
    }
}

class Region;

template class ChunkMap<Region*>;
template class ChunkMap<SynchedArea*>;
template class ChunkMap<ThreadArea*>;
template class ChunkMap<Chunk*>;
template class ChunkMap<int>;
template class ChunkMap<bool>;
