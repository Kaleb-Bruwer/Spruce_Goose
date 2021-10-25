#pragma once

#include <vector>

#include "../Slot.h"

class CraftingFrame{
public:
    CraftingFrame(){};
    CraftingFrame(short x, short y);

    short x, y;
    // index as frame[y][x]
    std::vector<std::vector<Slot>> frame;

    void trim(); //Removes whitespace on all sides
};
