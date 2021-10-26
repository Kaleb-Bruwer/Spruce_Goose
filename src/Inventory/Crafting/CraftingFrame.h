#pragma once

#include <vector>

#include "../Slot.h"

class CraftingFrame{
public:
    CraftingFrame(short x = 0, short y = 0);

    short x, y;
    // index as frame[y][x]
    std::vector<std::vector<Slot>> frame;

    void trim(); //Removes whitespace on all sides
};
