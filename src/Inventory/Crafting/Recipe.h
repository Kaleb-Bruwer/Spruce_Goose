#ifndef RECIPE_H
#define RECIPE_H

#include <string>
#include <vector>

#include "../Slot.h"
#include "CraftingFrame.h"


// Can be either a shaped or unshaped recipe, no inheritance
// This is done to reduce use of memory-references when searched
class Recipe{
public:
    bool shaped;

    std::vector<short> partitionIDs;

    short x, y;

    // Each element is a list of accepted Slots (can be multiple due to tags)
    // For shapeless this is 1d
    // For shaped, it is id pretending to be 2d (index = yi * x + xi)
    std::vector<std::vector<Slot>> pattern;

    Slot product;

    Recipe(){};

    void sortPartitionIDs();
    short partitionMatch(std::vector<short>& frame); //assume frame is already ordered

    bool match(CraftingFrame frame);
    Slot getProduct(){return product;};
};

long long hashPartitionIDs(std::vector<short> &partitionIDs);

#endif
