#include "Tag_Long.h"

using namespace std;

Tag_Long::Tag_Long(long long val) : NBT_Primitive<long long>(val){
    typeID = 4;
}

Tag_Long::Tag_Long(char* iterator, int &index, int bufferSize, bool hasName)
        : NBT_Primitive<long long>(iterator, index, bufferSize, hasName){
    typeID = 4;
}

string Tag_Long::printableStr(unsigned int nTabs){
    string result = "Long("+name+"): " + to_string(value) + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Long::Tag_Long(Tag_Long* rhs){
    NBT_Primitive<long long>::operator=(*rhs);
}

NBT* Tag_Long::clone(){
    Tag_Long* result = new Tag_Long();
    NBT_Primitive<long long>::operator=(*this);
    return result;
}
