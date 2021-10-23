#ifndef RECIPE_H
#define RECIPE_H

#include <string>
#include <vector>

#include "../Slot.h"
#include "CraftingFrame.h"


// Can be either a shaped or unshaped recipe, no inheritance
// This is done to reduce use of memory-references when searched
class Recipe{
protected:
    bool shaped;

    vector<short> partitionID;

    short x, y;

    // Each element is a list of accepted Slots (can be multiple due to tags)
    vector<Slot*> pattern; //if shaped, size = x*y; index = yi * x + xi

    Slot product;

public:
    Recipe(){};

    short partitionMatch(vector<short>& frame);

    bool match(CraftingFrame frame);
};

#endif
