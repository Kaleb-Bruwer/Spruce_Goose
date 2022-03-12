#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#include <map>
#include <vector>

#include "../Coordinate/ChunkCoord.h"
#include "../Coordinate/Coordinate.h"
#include "../../General/Functor.h"

using namespace std;

class ThreadArea;
class SynchedArea;
class Chunk;

template <class T>
class ChunkMap{
protected:
    map<ChunkCoord, T> values;

    vector<ChunkCoord> getRing(ChunkCoord center, int radius);
    vector<T> getRingValues(ChunkCoord center, int radius);

public:
    ChunkMap();
    virtual ~ChunkMap();

    T getVal(ChunkCoord c);
    T getVal(Coordinate<int> c);
    T getVal(Coordinate<double> c);
    void setVal(ChunkCoord c, T a);
    T getClosest(ChunkCoord c, int cutoff = 8);
    vector<T> getAllNearby(ChunkCoord c, int radius = 0);

    vector<T> getAll();
    vector<ChunkCoord> getAllPositions();

    //This assumes that all values are unique, it will execute multiple times on
    //duplicates
    void executeFunctor(Functor<T> &f);

    void copyTo(ChunkMap<T>* dest); //Overrides values in destinatiion
};

#endif
