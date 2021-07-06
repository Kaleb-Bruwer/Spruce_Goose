#include "GameSettings.h"

#include <fstream>
#include <iostream>

#include "../Datastructures/NBT/Tag_List.h"
#include "../Datastructures/NBT/Tag_String.h"

GameSettings* GameSettings::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new GameSettings();
        }
        constructMutex.unlock();
    }
    return instance;
}

Tag_Compound* GameSettings::getDimensionCodec(){
    if(dimensionCodec)
        return dimensionCodec;

    ifstream file("../Data/DimensionCodec.nbt");
    char array[4000];
    file.read(&(array[0]), 4000);
    int index = 0;
    dimensionCodec = new Tag_Compound(&(array[1]), index);

    return dimensionCodec;
}

Tag_Compound* GameSettings::getDimension(){
    if(dimension)
        return dimension;

    getDimensionCodec();

    //Get value[0]->element from dimensionCodec
    Tag_List* biomes = (Tag_List*) dimensionCodec->getItem("value");
    if(biomes){
        //Get dimension from dimensionCodec
        Tag_Compound* result = (Tag_Compound*) biomes->getValAt(0);
        if(result->getName() == ""){
            Tag_String* nameItem = (Tag_String*) result->getItem("name");
            result->setName(nameItem->getVal());
        }

        //Create reformatted dimension, client seems to do
        //some kind of key value mapping with this
        dimension = new Tag_Compound();
        Tag_String* key = new Tag_String("minecraft:dimension_type");
        key->setName("type");
        dimension->addItem(key);
        dimension->addItem(result);

        return dimension;
    }
    else{
        return 0;
    }
}
