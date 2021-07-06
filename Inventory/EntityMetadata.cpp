#include "EntityMetadata.h"

using namespace std;

unsigned char EntityMetaValBase::getDescriptor(){
    unsigned char result = 0;
    result = (index & 0x1F) | ((type << 5) & 0xE0);
    return result;
}

template<>
EntityMetaVal<char>::EntityMetaVal(char d, int i){
    data = d;
    type = 0;
    index = i;
}

template<>
EntityMetaVal<unsigned char>::EntityMetaVal(unsigned char d, int i){
    data = d;
    type = 0;
    index = i;
}

template<>
EntityMetaVal<short>::EntityMetaVal(short d, int i){
    data = d;
    type = 1;
    index = i;
}

template<>
EntityMetaVal<int>::EntityMetaVal(int d, int i){
    data = d;
    type = 2;
    index = i;
}

template<>
EntityMetaVal<float>::EntityMetaVal(float d, int i){
    data = d;
    type = 3;
    index = i;
}

template<>
EntityMetaVal<string>::EntityMetaVal(string d, int i){
    data = d;
    type = 4;
    index = i;
}

template<>
EntityMetaVal<Slot*>::EntityMetaVal(Slot* d, int i){
    data = d;
    type = 5;
    index = i;
}

template<>
EntityMetaVal<Coordinate<int>>::EntityMetaVal(Coordinate<int> d, int i){
    data = d;
    type = 6;
    index = i;
}

EntityMetadata::~EntityMetadata(){
    for(EntityMetaValBase* a : data){
        delete a;
    }
}
