#ifndef CRAFTING_H
#define CRAFTING_H

#include <mutex>
#include <vector>
#include <unordered_map>
#include <map>

#include "Recipe.h"
#include "CraftingFrame.h"
#include "../Slot.h"


class Tag_List;

// NOTE: hashPartitionIDs() declared and defined in Recipe.h and Recipe.cpp

class Crafting{
private:

    //vector<Recipe*> recipes;
    // multiset<Recipe> recipes;

    unordered_map<short, Slot> craftIDToSlot;
    unordered_map<short, short> craftIDToPartitionID;

    map<Slot, short> SlotToPartitionID;
    unordered_map<short, std::vector<Slot>> tagToSlots;

    // Use hashPartitionIDs() to get key
    unordered_map<long long, vector<Recipe>> recipes;

    inline static Crafting* instance;
    inline static mutex constructMutex;
    Crafting();
    ~Crafting();

    void readFromFile();
    void generateShapeless(Tag_List* shapeless);
    void generateShaped(Tag_List* shaped);

    vector<Slot> getValidSlots(short craftID);

public:
    static Crafting* getInstance();

    Slot getProduct(CraftingFrame &frame);

};

#endif
