#include "RegionMap.h"

using namespace std;

template <class T>
RegionMap<T>::RegionMap(){

}

template <class T>
RegionMap<T>::~RegionMap(){

}

template <class T>
T RegionMap<T>::getVal(RegionCoord r){
    ChunkCoord c;
    c.x = r.x;
    c.z = r.z;
    return ChunkMap<T>::getVal(c);
}

template <class T>
T RegionMap<T>::getVal(Coordinate<int> c){
    return getVal(c.getContainingRegion());
}

template <class T>
T RegionMap<T>::getVal(Coordinate<double> c){
    return getVal(c.getContainingRegion());
}

template <class T>
void RegionMap<T>::setVal(RegionCoord r, T val){
    ChunkCoord c;
    c.x = r.x;
    c.z = r.z;
    ChunkMap<T>::setVal(c, val);
}

template <class T>
T RegionMap<T>::getClosest(RegionCoord r, int cutoff){
    ChunkCoord c;
    c.x = r.x;
    c.z = r.z;

    return ChunkMap<T>::getClosest(c, cutoff);
}

template <class T>
vector<T> RegionMap<T>::getAllNearby(RegionCoord r, int radius){
    ChunkCoord c;
    c.x = r.x;
    c.z = r.z;

    return ChunkMap<T>::getAllNearby(c, radius);
}

template <class T>
vector<T> RegionMap<T>::getAll(){
    return ChunkMap<T>::getAll();
}

template <class T>
vector<RegionCoord> RegionMap<T>::getAllPositions(){
    vector<ChunkCoord> pos = ChunkMap<T>::getAllPositions();

    int pSize = pos.size();
    vector<RegionCoord> result(pSize);
    for(int i=0; i<pSize; i++){
        result[i].x = pos[i].x;
        result[i].z = pos[i].z;
    }
    return result;
}

template <class T>
vector<RegionCoord> RegionMap<T>::getRing(RegionCoord center, int radius){
    vector<RegionCoord> result;

    RegionCoord temp = center;
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
vector<T> RegionMap<T>::getRingValues(RegionCoord center, int radius){
    ChunkCoord cCenter;
    cCenter.x = center.x;
    cCenter.z = center.z;

    return ChunkMap<T>::getRingValues(cCenter, radius);
}

template <class T>
void RegionMap<T>::copyTo(RegionMap<T>* dest){
    ChunkMap<T>::copyTo(dest);
}

class Region;

template class RegionMap<SynchedArea*>;
template class RegionMap<Region*>;
template class RegionMap<int>;
template class RegionMap<bool>;
