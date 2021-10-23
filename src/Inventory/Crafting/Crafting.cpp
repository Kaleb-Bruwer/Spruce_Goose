#include "Crafting.h"

#include <fstream>
#include <iostream>

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
}


Slot Crafting::getProduct(CraftingFrame &frame){

    //Empty slot
    return Slot();
}
