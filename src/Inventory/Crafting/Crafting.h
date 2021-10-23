#ifndef CRAFTING_H
#define CRAFTING_H

#include <mutex>
#include <vector>
#include <set>

#include "Recipe.h"
#include "CraftingFrame.h"
#include "../Slot.h"


class Crafting{
private:

    //vector<Recipe*> recipes;
    // multiset<Recipe> recipes;

    inline static Crafting* instance;
    inline static mutex constructMutex;
    Crafting();
    ~Crafting();

    void readFromFile();

public:
    static Crafting* getInstance();

    Slot getProduct(CraftingFrame &frame);

};

#endif
