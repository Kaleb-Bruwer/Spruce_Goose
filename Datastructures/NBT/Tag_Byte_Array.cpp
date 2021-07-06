#include "Tag_Byte_Array.h"

#include <cstring>

Tag_Byte_Array::Tag_Byte_Array(char* iterator, int &index, bool hasName)
        : NBT_Array<char>(iterator, index, hasName){
    typeID = 7;
}

string Tag_Byte_Array::printableStr(unsigned int nTabs){
    string result = "Byte_Array("+name+"): ";
    for(int i=0; i<size-1; i++){
        result += to_string((int) array[i]) + ", ";
    }
    result += to_string((int) array[size-1]) + "\n";

    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Byte_Array::Tag_Byte_Array(Tag_Byte_Array* rhs){
    NBT_Array<char>::operator=(*rhs);
}

NBT* Tag_Byte_Array::clone(){
    return new Tag_Byte_Array(this);
}
