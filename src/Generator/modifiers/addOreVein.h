#ifndef ADDOREVEIN_H
#define ADDOREVEIN_H

#include "../../Datastructures/Coordinate/Coordinate.h"
#include "../../World/Block.h"

using namespace std;

class Chunk;

//Yes, this is a function and not a class. Get over it.

void addOreVein(Chunk* c, Coordinate<int> pos, int size, Block b);

#endif
