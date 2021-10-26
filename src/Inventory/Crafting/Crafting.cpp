#include "Crafting.h"

#include <fstream>
#include <iostream>

#include "../../Datastructures/NBT/Tag_List.h"
#include "../../Datastructures/NBT/Tag_Short.h"

Crafting* Crafting::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new Crafting();
        }
        constructMutex.unlock();
    }
    return instance;
}

Crafting::Crafting(){
    readFromFile();

    cout << "31: " << craftIDToPartitionID[31] << endl;
    cout << "431: " << craftIDToPartitionID[431] << endl;

}

Crafting::~Crafting(){
    //Probably won't ever run since this is a singleton
}

void Crafting::readFromFile(){
    string filename = "../Data/recipes.nbt";
    ifstream file(filename);

    char* array = new char[64000];
    file.read(array, 64000);

    int index =0;
    Tag_Compound tc(array+1,index);
    file.close();

    // Populate craftIDToSlot
    Tag_List* items = (Tag_List*) tc.getItem("items");

    int numItems = items->getSize();

    // Used to iterate over later
    vector<short> craftIDs(numItems);

    for(int i=0; i<numItems; i++){
        Tag_Compound* item = (Tag_Compound*) items->getValAt(i);

        short craftID = ((Tag_Short*)item->getItem("craftID"))->getVal();
        short itemID = ((Tag_Short*)item->getItem("itemID"))->getVal();
        short metaVal = ((Tag_Short*)item->getItem("metaVal"))->getVal();

        Slot slot = Slot(itemID, metaVal);
        craftIDToSlot[craftID] = slot;

        craftIDs[i] = craftID;
    }

    // Populate craftIDToPartitionID
    Tag_List* partitions = (Tag_List*) tc.getItem("partitions");

    int numPartitions = partitions->getSize();
    for(int i=0; i<numPartitions; i++){
        Tag_List* list = (Tag_List*) partitions->getValAt(i);

        int partSize = list->getSize();
        for(int j=0; j<partSize; j++){
            short craftID = ((Tag_Short*)list->getValAt(j))->getVal();
            craftIDToPartitionID[craftID] = i;
        }
    }

    Tag_List* tags = (Tag_List*) tc.getItem("tags");

    int numTags = tags->getSize();
    for(int i=0; i<numTags; i++){
        Tag_Compound* tag = (Tag_Compound*) tags->getValAt(i);

        short craftID = ((Tag_Short*)tag->getItem("craftID"))->getVal();
        Tag_List* items = (Tag_List*) tag->getItem("items");

        int n = items->getSize();
        vector<Slot> slots(n);
        for(int j=0; j<n; j++){
            short c = ((Tag_Short*)items->getValAt(j))->getVal();
            slots[j] = craftIDToSlot[c];
        }

        tagToSlots[craftID] = slots;
    }


    // Populate SlotToPartitionID
    // Iterates over a vector of all craftIDs that represent items
    for(int i=0; i<craftIDs.size(); i++){
        short c = craftIDs[i];
        SlotToPartitionID[craftIDToSlot[c]] = craftIDToPartitionID[c];
    }

    generateShapeless((Tag_List*) tc.getItem("shapeless"));
    generateShaped((Tag_List*) tc.getItem("shaped"));
}

void Crafting::generateShapeless(Tag_List *shapeless){
    int numRecipes = shapeless->getSize();

    for(int i=0; i<numRecipes; i++){
        Recipe recipe;
        recipe.shaped = false;

        Tag_Compound* recipeNBT = (Tag_Compound*)shapeless->getValAt(i);

        Tag_List* items = (Tag_List*)recipeNBT->getItem("items");
        short result = ((Tag_Short*)recipeNBT->getItem("result"))->getVal();
        short resultCount = ((Tag_Short*)recipeNBT->getItem("resultCount"))->getVal();

        recipe.product = craftIDToSlot[result];
        recipe.product.itemCount = resultCount;

        int n = items->getSize();
        recipe.pattern.resize(n);
        vector<short> partitionIDs(n, 0);

        for(int j=0; j<n; j++){
            short craftID = ((Tag_Short*)items->getValAt(j))->getVal();

            recipe.pattern[j] = getValidSlots(craftID);
            partitionIDs[j] = craftIDToPartitionID[craftID];
        }
        selectSort(partitionIDs);
        long long key = hashPartitionIDs(partitionIDs);

        recipes[key].push_back(recipe);
    }
}

void Crafting::generateShaped(Tag_List* shaped){
    int numRecipes = shaped->getSize();

    for(int i=0; i<numRecipes; i++){
        Recipe recipe;
        recipe.shaped = true;

        Tag_Compound* recipeNBT = (Tag_Compound*)shaped->getValAt(i);

        short result = ((Tag_Short*)recipeNBT->getItem("result"))->getVal();
        short resultCount = ((Tag_Short*)recipeNBT->getItem("resultCount"))->getVal();

        recipe.product = craftIDToSlot[result];
        recipe.product.itemCount = resultCount;

        Tag_List* yNBT = (Tag_List*)recipeNBT->getItem("y");
        int yS = yNBT->getSize();
        int xS = -1;
        recipe.y = yS;

        vector<short> partitionIDs;

        for(int j=0; j<yS; j++){
            Tag_List* xNBT = (Tag_List*)yNBT->getValAt(j);
            if(xS == -1){
                xS = xNBT->getSize();
                recipe.pattern.resize(xS * yS);
            }
            recipe.x = xS;

            for(int k=0; k<xS; k++){
                int index = j*xS + k;

                short craftID = ((Tag_Short*)xNBT->getValAt(k))->getVal();
                if(craftID != -1){
                    recipe.pattern[index] = getValidSlots(craftID);
                    partitionIDs.push_back(craftIDToPartitionID[craftID]);
                }
            }
        }
        selectSort(partitionIDs);

        long long key = hashPartitionIDs(partitionIDs);
        recipes[key].push_back(recipe);

    }
}

vector<Slot> Crafting::getValidSlots(short craftID){
    auto it = craftIDToSlot.find(craftID);
    if(it == craftIDToSlot.end()){
        // craftID is a tag
        return tagToSlots[craftID];
    }
    else{
        // craftID is an item
        return vector<Slot>{craftIDToSlot[craftID]};
    }
}

long long Crafting::hashCraftingFrame(CraftingFrame &frame){
    // First get partition ids
    vector<short> partitionIDs;

    for(int i=0; i<frame.y; i++){
        for(int j=0; j<frame.x; j++){
            if(!frame.frame[i][j].isEmpty()){
                partitionIDs.push_back(SlotToPartitionID[frame.frame[i][j]]);
            }
        }
    }

    // Sort
    selectSort(partitionIDs);

    return hashPartitionIDs(partitionIDs);
}

Slot Crafting::getProduct(CraftingFrame frame){
    frame.trim();
    frame.dropCounts();
    long long key = hashCraftingFrame(frame);

    auto it = recipes.find(key);
    if(it == recipes.end())
        return Slot(); //No matching recipe exists

    for(int i=0; i<it->second.size(); i++){
        if(it->second[i].match(frame))
            return it->second[i].getProduct();
    }

    //Empty slot
    return Slot();
}
