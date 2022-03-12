#ifndef RECIPE_H
#define RECIPE_H

#include <string>
#include <vector>

#include "../Slot.h"
#include "CraftingFrame.h"

void selectSort(std::vector<short> &arr);
long long hashPartitionIDs(std::vector<short> &partitionIDs);

// Can be either a shaped or unshaped recipe, no inheritance
// This is done to reduce use of memory-references when searched
class Recipe{
private:
    bool shapelessMatch(CraftingFrame &frame, vector<bool> matched, short xi, short yi);

public:
    bool shaped;

    short x, y;

    // Each element is a list of accepted Slots (can be multiple due to tags)
    // For shapeless this is 1d
    // For shaped, it is id pretending to be 2d (index = yi * x + xi)
    std::vector<std::vector<Slot>> pattern;

    Slot product;

    Recipe(){};

    // short partitionMatch(std::vector<short>& frame); //assume frame is already ordered

    bool match(CraftingFrame &frame);
    Slot getProduct(){return product;};
};


#endif
