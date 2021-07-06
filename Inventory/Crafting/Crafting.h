#ifndef CRAFTING_H
#define CRAFTING_H

#include <mutex>
#include <vector>
#include <set>

#include "Recipe.h"
#include "../Slot.h"

using namespace std;

class Crafting{
private:

    //vector<Recipe*> recipes;
    multiset<Recipe> recipes;

    inline static Crafting* instance;
    inline static mutex constructMutex;
    Crafting();
    ~Crafting();

    void readFromFile();

public:
    static Crafting* getInstance();

    vector<Recipe> getAllRecipes();
    Slot getProduct(ShapedRecipe* c);

};

#endif
