#ifndef NBT_ARRAY_H
#define NBT_ARRAY_H

#include "NBT.h"

using namespace std;

template <class T>
class NBT_Array : public NBT{
protected:
    int size = 0;
    T* array = 0;
public:
    NBT_Array(){};
    NBT_Array(char* iterator, int &index, int bufferSize, bool hasName = true);
    virtual ~NBT_Array();
    NBT_Array<T>& operator=(const NBT_Array<T>& rhs);

    void setArray(T* arr, int size); //Shallow copy
    T* getArray(){return array;};
    T getValAt(int index);
    int getSize(){return size;};

    int getTransmitSize();
    void getTransmittable(char* buffer, int &index);
};

#endif
