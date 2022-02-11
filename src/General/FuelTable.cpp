#include "FuelTable.h"

#include <fstream>
#include <iostream>

#include "../Datastructures/NBT/Tag_Compound.h"
#include "../Datastructures/NBT/Tag_List.h"
#include "../Datastructures/NBT/Tag_Short.h"

FuelTable* FuelTable::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new FuelTable();
        }
        constructMutex.unlock();
    }
    return instance;
}

FuelTable::FuelTable(){
    readFromFile();
}


void FuelTable::readFromFile(){
    string filename = "../Data/fuels.nbt";
    ifstream file(filename);

    // Increase this buffer size if fuels.nbt ever grows significantly
    char* array = new char[4096];
    file.read(array, 4096);

    int index = 0;
    Tag_Compound tc(array+1,index);
    file.close();

    //THIS PTR DOESN'T OWN THIS MEMORY, ONLY REFERENCES IT
    Tag_List* fuels = (Tag_List*) tc.getItem("fuels");

    int numItems = fuels->getSize();

    for(int i=0; i<numItems; i++){
        Tag_Compound* f = (Tag_Compound*) fuels->getValAt(i);

        short itemID = ((Tag_Short*)f->getItem("itemID"))->getVal();
        // Using short for duration is cutting it close, since its highest
        // value is 20000
        // If better fuels are introduced, this must be made an int
        short duration = ((Tag_Short*)f->getItem("duration"))->getVal();

        itemIDToDuration[itemID] = duration;
    }
}

int FuelTable::getDuration(int itemID){
    auto it = itemIDToDuration.find(itemID);
    if(it == itemIDToDuration.end())
        return 0;
    else
        return it->second;
}
