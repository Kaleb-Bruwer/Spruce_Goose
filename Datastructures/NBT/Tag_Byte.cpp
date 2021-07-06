#include "Tag_Byte.h"

using namespace std;

Tag_Byte::Tag_Byte(char val) : NBT_Primitive<char>(val){
    typeID = 1;
}

Tag_Byte::Tag_Byte(char* iterator, int &index, bool hasName)
        : NBT_Primitive<char>(iterator, index, hasName){
    typeID = 1;
}

string Tag_Byte::printableStr(unsigned int nTabs){
    string result = "Byte("+name+"): " + to_string((int)value) + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Byte::Tag_Byte(Tag_Byte* rhs){
    NBT_Primitive<char>::operator=(*rhs);
}

NBT* Tag_Byte::clone(){
    Tag_Byte* result = new Tag_Byte();
    NBT_Primitive<char>::operator=(*this);
    return result;
}
