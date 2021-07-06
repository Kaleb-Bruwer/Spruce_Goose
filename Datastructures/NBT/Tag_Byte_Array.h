#ifndef TAG_BYTE_ARRAY_H
#define TAG_BYTE_ARRAY_H

#include "NBT_Array.h"

using namespace std;

class Tag_Byte_Array : public NBT_Array<char>{
public:
    Tag_Byte_Array(){typeID = 7;};
    Tag_Byte_Array(char* iterator, int &index, bool hasName = true);
    Tag_Byte_Array(Tag_Byte_Array* rhs);
    NBT* clone();

    string printableStr(unsigned int nTabs = 0);
};

#endif
