#include "Tag_Float.h"

using namespace std;

Tag_Float::Tag_Float(float val) : NBT_Primitive<float>(val){
    typeID = 5;
}

Tag_Float::Tag_Float(char* iterator, int &index, int bufferSize, bool hasName)
        : NBT_Primitive<float>(iterator, index, bufferSize, hasName){
    typeID = 5;
}

string Tag_Float::printableStr(unsigned int nTabs){
    string result = "Float("+name+"): " + to_string(value) + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Float::Tag_Float(Tag_Float* rhs){
    NBT_Primitive<float>::operator=(*rhs);
}

NBT* Tag_Float::clone(){
    Tag_Float* result = new Tag_Float();
    NBT_Primitive<float>::operator=(*this);
    return result;
}
