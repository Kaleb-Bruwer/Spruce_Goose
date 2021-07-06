#ifndef REGIONMAP_H
#define REGIONMAP_H

#include "ChunkMap.h"

#include "../Coordinate/RegionCoord.h"


//Provides an interface to the ChunkMap class that uses Region coords
template <class T>
class RegionMap : private ChunkMap<T>{
protected:
    vector<RegionCoord> getRing(RegionCoord center, int radius);
    vector<T> getRingValues(RegionCoord center, int radius);

public:
    RegionMap();
    ~RegionMap();

    T getVal(RegionCoord c);
    T getVal(Coordinate<int> c);
    T getVal(Coordinate<double> c);

    void setVal(RegionCoord c, T val);
    T getClosest(RegionCoord c, int cutoff = 0);

    vector<T> getAllNearby(RegionCoord r, int radius = 0);
    vector<T> getAll();
    vector<RegionCoord> getAllPositions();

    void copyTo(RegionMap<T>* dest);
};

#endif
