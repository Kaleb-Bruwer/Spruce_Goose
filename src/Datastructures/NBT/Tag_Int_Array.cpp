#include "Tag_Int_Array.h"

#include <cstring>

Tag_Int_Array::Tag_Int_Array(char* iterator, int &index, int bufferSize, bool hasName)
        : NBT_Array<int>(iterator, index, bufferSize, hasName){
    typeID = 11;
}

string Tag_Int_Array::printableStr(unsigned int nTabs){
    string result = "Int_Array("+name+"): ";
    for(int i=0; i<size-1; i++){
        result += to_string(array[i]) + ", ";
    }
    result += to_string(array[size-1]) + "\n";

    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Int_Array::Tag_Int_Array(Tag_Int_Array* rhs){
    NBT_Array<int>::operator=(*rhs);
}

NBT* Tag_Int_Array::clone(){
    return new Tag_Int_Array(this);
}
