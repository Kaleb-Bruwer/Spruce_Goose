#ifndef ENTITYMETADATA_H
#define ENTITYMETADATA_H

#include <string>
#include <iostream>

#include "Slot.h"
#include "../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class EntityMetaValBase{
public:
    short index;
    short type;

    unsigned char getDescriptor();//index and type combined into 1 byte
};

//Note that Slot must be a pointer

template <class T>
class EntityMetaVal : public EntityMetaValBase{
public:
    T data;

    EntityMetaVal(T d, int index);
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//EntityMetadata is the class you'll actually use, the previous 2 are helpers

class EntityMetadata{
public:
    vector<EntityMetaValBase*> data;

    ~EntityMetadata();

    template <class T>
    void addVal(int index, T val){
        data.push_back(new EntityMetaVal<T>(val, index));
    };
};

template class EntityMetaVal<char>;
template class EntityMetaVal<unsigned char>;
template class EntityMetaVal<short>;
template class EntityMetaVal<int>;
template class EntityMetaVal<float>;
template class EntityMetaVal<string>;
template class EntityMetaVal<Slot*>;
template class EntityMetaVal<Coordinate<int>>;

#endif
