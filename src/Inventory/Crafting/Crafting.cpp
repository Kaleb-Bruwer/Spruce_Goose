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
    // cout << tc.printableStr() << endl;
    file.close();

    // Populate craftIDToSlot
    Tag_List* items = (Tag_List*) tc.getItem("items");
    // cout << items->printableStr() << endl;

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
    cout << "NUMPARTITIONS: " << numPartitions << endl;
    cout << partitions->printableStr() << endl;
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
            slots[j] = ((Tag_Short*)items->getValAt(j))->getVal();
        }

        tagToSlots[i] = slots;
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

        Tag_Compound* recipeNBT = (Tag_Compound*)shapeless->getValAt(i);

        Tag_List* items = (Tag_List*)recipeNBT->getItem("items");
        short result = ((Tag_Short*)recipeNBT->getItem("result"))->getVal();
        short resultCount = ((Tag_Short*)recipeNBT->getItem("resultCount"))->getVal();

        recipe.product = craftIDToSlot[result];
        recipe.product.itemCount = resultCount;

        int n = items->getSize();
        recipe.pattern.resize(n);
        recipe.partitionIDs.resize(n);
        for(int j=0; j<n; j++){
            short craftID = ((Tag_Short*)items->getValAt(j))->getVal();

            recipe.pattern[j] = getValidSlots(craftID);
            recipe.partitionIDs[j] = craftIDToPartitionID[craftID];
        }
    }
}

void Crafting::generateShaped(Tag_List* shaped){
    int numRecipes = shaped->getSize();

    for(int i=0; i<numRecipes; i++){
        Recipe recipe;

        Tag_Compound* recipeNBT = (Tag_Compound*)shaped->getValAt(i);

        short result = ((Tag_Short*)recipeNBT->getItem("result"))->getVal();
        short resultCount = ((Tag_Short*)recipeNBT->getItem("resultCount"))->getVal();

        recipe.product = craftIDToSlot[result];
        recipe.product.itemCount = resultCount;

        Tag_List* yNBT = (Tag_List*)recipeNBT->getItem("y");
        int yS = yNBT->getSize();
        int xS = -1;
        recipe.y = yS;

        for(int j=0; j<yS; j++){
            Tag_List* xNBT = (Tag_List*)yNBT->getValAt(j);
            if(xS == -1){
                xS = xNBT->getSize();
                recipe.pattern.resize(xS * yS);
                recipe.partitionIDs.resize(xS * yS);

            }
            recipe.x = xS;

            for(int k=0; k<xS; k++){
                int index = j*xS + k;

                short craftID = ((Tag_Short*)xNBT->getValAt(k))->getVal();
                recipe.pattern[index] = getValidSlots(craftID);
                recipe.partitionIDs[index] = craftIDToPartitionID[craftID];
            }
        }
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

Slot Crafting::getProduct(CraftingFrame &frame){

    //Empty slot
    return Slot();
}
