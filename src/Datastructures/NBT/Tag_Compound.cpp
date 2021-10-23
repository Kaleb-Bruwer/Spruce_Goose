#include "Tag_Compound.h"

#include <iostream>

#include "Tag_Byte.h"
#include "Tag_Short.h"
#include "Tag_Int.h"
#include "Tag_Long.h"
#include "Tag_Float.h"
#include "Tag_Double.h"
#include "Tag_Byte_Array.h"
#include "Tag_String.h"
#include "Tag_List.h"
#include "Tag_Int_Array.h"
#include "Tag_Long_Array.h"

using namespace std;

Tag_Compound::Tag_Compound(){
    //Supposed to do nothing
}

Tag_Compound::Tag_Compound(char* iterator, int &index, bool hasName) : NBT(hasName){
    //Create from raw data
    if(hasName)
        readRawName(iterator, index);

    while(true){
        //Breaks loop when a Tag_End shows up
        unsigned short type = iterator[index]; //1 byte
        index++;
        if(type == 0)
            break; //Tag_End
        addItem(iterator, index, type);
    }
}

Tag_Compound::Tag_Compound(Tag_Compound* rhs){
    *this = *rhs;
}

Tag_Compound::~Tag_Compound(){
    for(auto it = children.begin(); it != children.end(); it++)
        delete (*it);
}

NBT* Tag_Compound::clone(){
    Tag_Compound* result = new Tag_Compound();
    (*result) = (*this);
    return result;
}

Tag_Compound& Tag_Compound::operator=(const Tag_Compound& rhs){
    NBT::operator=(rhs);

    //First clear existing data, if any
    int size = children.size();
    for(int i=0; i<size; i++){
        delete children[i];
    }

    //Now deep copy data from rhs
    size = rhs.children.size();
    children.resize(size);
    for(int i=0; i<size; i++){
        children[i] = rhs.children[i]->clone();
    }
}

void Tag_Compound::addItem(char* iterator, int &index, unsigned short int type){
    switch(type){
        case 1:
            children.push_back(new Tag_Byte(iterator, index));
            break;
        case 2:
            children.push_back(new Tag_Short(iterator, index));
            break;
        case 3:
            children.push_back(new Tag_Int(iterator, index));
            break;
        case 4:
            children.push_back(new Tag_Long(iterator, index));
            break;
        case 5:
            children.push_back(new Tag_Float(iterator, index));
            break;
        case 6:
            children.push_back(new Tag_Double(iterator, index));
            break;
        case 7:
            children.push_back(new Tag_Byte_Array(iterator, index));
            break;
        case 8:
            children.push_back(new Tag_String(iterator, index));
            break;
        case 9:
            children.push_back(new Tag_List(iterator, index));
            break;
        case 10:
            children.push_back(new Tag_Compound(iterator, index));
            break;
        case 11:
            children.push_back(new Tag_Int_Array(iterator, index));
            break;
        case 12:
            children.push_back(new Tag_Long_Array(iterator, index));
            break;
    }
}

NBT* Tag_Compound::getItem(string name){
    for(NBT* item : children){
        if(item->getName() == name)
            return item;
    }
    return 0;
}

void Tag_Compound::addItem(NBT* item){
    children.push_back(item);
}

string Tag_Compound::printableStr(unsigned int nTabs){
    string result = "Compound("+name+"):\n";

    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }

    for(int i=0; i<children.size(); i++){
        result += children[i]->printableStr(nTabs+1);
    }
    return result;
}

int Tag_Compound::getTransmitSize(){
    int result = sizeNoData() +1;//1 for tag_End
    for(auto it=children.begin(); it != children.end(); it++){
        result += (*it)->getTransmitSize();
    }

    result += children.size(); //Type bytes
    return result;
}

void Tag_Compound::getTransmittable(char* buffer, int &index){
    if(hasName){
        buffer[index] = (char)10;
        index++;
        writeName(buffer, index);
    }

    for(auto it = children.begin(); it != children.end(); it++){
        (*it)->getTransmittable(buffer, index);
    }
    buffer[index] = 0;
    index++;
}
