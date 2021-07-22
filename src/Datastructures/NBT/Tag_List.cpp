#include "Tag_List.h"

#include "Tag_Byte.h"
#include "Tag_Short.h"
#include "Tag_Int.h"
#include "Tag_Long.h"
#include "Tag_Float.h"
#include "Tag_Double.h"
#include "Tag_Byte_Array.h"
#include "Tag_String.h"
#include "Tag_Compound.h"
#include "Tag_Int_Array.h"
#include "Tag_Long_Array.h"

#include <cstring>

Tag_List::Tag_List(int t, int s){
    typeID = 9;
    type = t;
    size = s;
    value = new NBT*[s];
    for(int i=0; i<s; i++)
        value[i] = 0;
}

Tag_List::Tag_List(char* iterator, int &index, bool hasName) : NBT(hasName){
    typeID = 9;
    if(hasName)
        readRawName(iterator, index);
    //Get TypeID of elements
    char typeC = iterator[index];
    type = (unsigned int) typeC;
    index++;

    //Get length
    void* sizePtr = (void*) &size;
    memcpy(sizePtr, &(iterator[index]), 4);
    switchEndian(sizePtr,4);
    index += 4;

    value = new NBT*[size];

    for(int i=0; i<size; i++){
        addItem(iterator, index, i);
    }
}

Tag_List::Tag_List(Tag_List* rhs){
    *this = *rhs;
}

Tag_List::~Tag_List(){
    for(int i=0; i<size; i++)
        delete value[i];
    delete [] value;
}

NBT* Tag_List::clone(){
    Tag_List* result = new Tag_List();
    *result = *this;
    return result;
}

Tag_List& Tag_List::operator=(const Tag_List& rhs){
    NBT::operator=(rhs);

    if(value){
        for(int i=0; i<currSize; i++){
            delete value[i];
        }
        delete [] value;
    }

    type = rhs.type;
    currSize = rhs.currSize;
    size = rhs.size;
    value = new NBT*[size];

    for(int i=0; i<currSize; i++){
        value[i] = rhs.value[i]->clone();
    }
}

NBT* Tag_List::getValAt(int index){
    if(index <0 || index >= size)
        return 0;
    return value[index];
}

void Tag_List::addItem(char* iterator, int &index, int pos){
    //Call nameless constructor
    switch(type){
        case 1:
            value[pos] = new Tag_Byte(iterator, index, false);
            break;
        case 2:
            value[pos] = new Tag_Short(iterator, index, false);
            break;
        case 3:
            value[pos] = new Tag_Int(iterator, index, false);
            break;
        case 4:
            value[pos] = new Tag_Long(iterator, index, false);
            break;
        case 5:
            value[pos] = new Tag_Float(iterator, index, false);
            break;
        case 6:
            value[pos] = new Tag_Double(iterator, index, false);
            break;
        case 7:
            value[pos] = new Tag_Byte_Array(iterator, index, false);
            break;
        case 8:
            value[pos] = new Tag_String(iterator, index, false);
            break;
        case 9:
            value[pos] = new Tag_List(iterator, index, false);
            break;
        case 10:
            value[pos] = new Tag_Compound(iterator, index, false);
            break;
        case 11:
            value[pos] = new Tag_Int_Array(iterator, index, false);
            break;
        case 12:
            value[pos] = new Tag_Long_Array(iterator, index, false);
            break;
    }
}

void Tag_List::addItem(NBT* item){
    if(currSize < size){
        value[currSize] = item;
        currSize++;
        item->setHasName(false);
    }
}

string Tag_List::printableStr(unsigned int nTabs){
    string result = "List("+name+"):\n";

    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }

    for(int i=0; i<size; i++){
        result += value[i]->printableStr(nTabs+1);
    }
    return result;
}

int Tag_List::getTransmitSize(){
    // 4 (for size), type already included in sizeNoData()
    int result = sizeNoData() + 4;
    for(int i=0; i<size; i++){
        result += value[i]->getTransmitSize();
    }
    return result;
}

void Tag_List::getTransmittable(char* buffer, int &index){
    if(hasName){
        buffer[index] = (char)9;
        index++;
        writeName(buffer,index);
    }

    buffer[index] = (char)type;
    index++;

    void* a = &size;
    memcpy(&(buffer[index]), a, 4);
    switchEndian(&(buffer[index]),4);
    index += 4;

    for(int i=0; i<size; i++){
        value[i]->getTransmittable(buffer,index);
    }
}
