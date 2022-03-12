#include "Tag_Long_Array.h"

#include <cstring>


Tag_Long_Array::Tag_Long_Array(char* iterator, int &index, int bufferSize, bool hasName)
        : NBT_Array<long long>(iterator, index, bufferSize, hasName){
    typeID = 12;
}

string Tag_Long_Array::printableStr(unsigned int nTabs){
    string result = "Long_Array("+name+"): ";
    for(int i=0; i<size-1; i++){
        result += to_string(array[i]) + ", ";
    }
    result += to_string(array[size-1]) + "\n";

    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Long_Array::Tag_Long_Array(Tag_Long_Array* rhs){
    NBT_Array<long long>::operator=(*rhs);
}

NBT* Tag_Long_Array::clone(){
    return new Tag_Long_Array(this);
}
