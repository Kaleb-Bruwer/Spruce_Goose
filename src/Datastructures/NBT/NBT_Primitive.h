#ifndef NBT_PRIMITIVE_H
#define NBT_PRIMITIVE_H

#include "NBT.h"

using namespace std;

template <class T>
class NBT_Primitive : public NBT{
protected:
    T value;
public:
    NBT_Primitive(){};
    NBT_Primitive(T);
    NBT_Primitive(char* iterator, int &index, bool hasName = true);
    NBT_Primitive<T>& operator=(const NBT_Primitive<T>& rhs);

    T getVal(){return value;};
    void setVal(T v){value = v;};

    int getTransmitSize();
    void getTransmittable(char* buffer, int &index);
};

#endif
