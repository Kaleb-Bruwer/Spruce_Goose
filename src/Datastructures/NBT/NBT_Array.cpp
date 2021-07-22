#include "NBT_Array.h"

#include <cstring>
#include <iostream>

using namespace std;

template <class T>
NBT_Array<T>::NBT_Array(char* iterator, int &index, bool hasName) : NBT(hasName){
    if(hasName){
        readRawName(iterator, index);
    }
    iterator += index;

    memcpy((void*)&size, iterator, 4);
    switchEndian((void*)&size, 4);
    iterator += 4;
    index += 4;

    array = new T[size];
    memcpy(array, iterator, sizeof(T) * size);
    index += sizeof(T) * size;

    for(int i=0; i<size; i++){
        switchEndian((void*)&(array[size]), sizeof(T));
    }
}

template <class T>
NBT_Array<T>::~NBT_Array(){
    if(array)
        delete [] array;
}

template <class T>
NBT_Array<T>& NBT_Array<T>::operator=(const NBT_Array<T>& rhs){
    NBT::operator=(rhs);

    size = rhs.size;
    array = new T[size];
    memcpy(array, rhs.array, sizeof(array));
}

template <class T>
void NBT_Array<T>::setArray(T* arr, int s){
    array = arr;
    size = s;
}

template <class T>
T NBT_Array<T>::getValAt(int index){
    if(index < 0 || index >= size)
        return 0;
    return array[index];
}


template <class T>
int NBT_Array<T>::getTransmitSize(){
    return (sizeNoData() + sizeof(T)*size + 4);
}

template <class T>
void NBT_Array<T>::getTransmittable(char* buffer, int &index){
    int pre = index;
    if(hasName){
        buffer[index] = (char)typeID;
        index++;
        writeName(buffer,index);
    }

    memcpy(&(buffer[index]), (void*)&size, 4);
    switchEndian(&(buffer[index]), 4);
    index+=4;

    memcpy(&(buffer[index]), array, sizeof(T)*size);
    for(int i=0; i<size; i++){
        switchEndian(&(buffer[index]), sizeof(T));
        index += sizeof(T);
    }
}


template class NBT_Array<char>;
template class NBT_Array<int>;
template class NBT_Array<long long>;
