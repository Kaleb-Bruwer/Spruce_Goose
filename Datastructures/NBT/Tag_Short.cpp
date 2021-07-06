#include "Tag_Short.h"

using namespace std;

Tag_Short::Tag_Short(short val) : NBT_Primitive<short>(val){
    typeID = 2;
}

Tag_Short::Tag_Short(char* iterator, int &index, bool hasName)
        : NBT_Primitive<short>(iterator, index, hasName){
    typeID = 2;
}

string Tag_Short::printableStr(unsigned int nTabs){
    string result = "Short("+name+"): " + to_string(value) + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Short::Tag_Short(Tag_Short* rhs){
    NBT_Primitive<short>::operator=(*rhs);
}

NBT* Tag_Short::clone(){
    Tag_Short* result = new Tag_Short();
    NBT_Primitive<short>::operator=(*this);
    return result;
}
