#include "Tag_Int.h"

using namespace std;

Tag_Int::Tag_Int(int val) : NBT_Primitive<int>(val){
    typeID = 3;
}

Tag_Int::Tag_Int(char* iterator, int &index, int bufferSize, bool hasName)
        : NBT_Primitive<int>(iterator, index, bufferSize, hasName){
    typeID = 3;
}

string Tag_Int::printableStr(unsigned int nTabs){
    string result = "Int("+name+"): " + to_string(value) + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Int::Tag_Int(Tag_Int* rhs){
    NBT_Primitive<int>::operator=(*rhs);
}

NBT* Tag_Int::clone(){
    Tag_Int* result = new Tag_Int();
    NBT_Primitive<int>::operator=(*this);
    return result;
}
