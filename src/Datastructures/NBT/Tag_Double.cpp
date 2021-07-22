#include "Tag_Double.h"

using namespace std;

Tag_Double::Tag_Double(double val) : NBT_Primitive<double>(val){
    typeID = 6;
}

Tag_Double::Tag_Double(char* iterator, int &index, bool hasName)
        : NBT_Primitive<double>(iterator, index, hasName){
    typeID = 6;
}

string Tag_Double::printableStr(unsigned int nTabs){
    string result = "Double("+name+"): " + to_string(value) + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

Tag_Double::Tag_Double(Tag_Double* rhs){
    NBT_Primitive<double>::operator=(*rhs);
}

NBT* Tag_Double::clone(){
    Tag_Double* result = new Tag_Double();
    NBT_Primitive<double>::operator=(*this);
    return result;
}
