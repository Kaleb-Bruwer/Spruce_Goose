#include "NBT_Primitive.h"

#include <cstring>

#include "../../Protocol/ReadFromBuffer.h"

using namespace std;

template <class T>
NBT_Primitive<T>::NBT_Primitive(T val){
    value = val;
}

template <class T>
NBT_Primitive<T>::NBT_Primitive(char* iterator, int &index, int bufferSize,
        bool hasName) : NBT(hasName){
    if(hasName){
        readRawName(iterator, index, bufferSize);
    }
    value = ReadFromBuffer::read<T>(iterator, index, bufferSize);
}

template <class T>
NBT_Primitive<T>& NBT_Primitive<T>::operator=(const NBT_Primitive<T>& rhs){
    NBT::operator=(rhs);

    value = rhs.value;
}

template <class T>
int NBT_Primitive<T>::getTransmitSize(){
    return (sizeNoData() + sizeof(value));
}

template <class T>
void NBT_Primitive<T>::getTransmittable(char* buffer, int &index){
    if(hasName){
        buffer[index] = (char)typeID;
        index++;
        writeName(buffer, index);
    }

    memcpy(&(buffer[index]), (void*)&value, sizeof(value));
    switchEndian(&(buffer[index]), sizeof(value));
    index += sizeof(value);
}

template class NBT_Primitive<char>;
template class NBT_Primitive<short>;
template class NBT_Primitive<int>;
template class NBT_Primitive<long long>;
template class NBT_Primitive<float>;
template class NBT_Primitive<double>;
