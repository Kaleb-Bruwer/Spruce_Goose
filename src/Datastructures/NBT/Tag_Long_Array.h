#ifndef TAG_LONG_ARRAY_H
#define TAG_LONG_ARRAY_H

#include "NBT_Array.h"

using namespace std;

class Tag_Long_Array : public NBT_Array<long long>{
public:
    Tag_Long_Array(){typeID = 12;};
    Tag_Long_Array(char* iterator, int &index, bool hasName = true);
    Tag_Long_Array(Tag_Long_Array* rhs);
    NBT* clone();

    string printableStr(unsigned int nTabs = 0);
};

#endif
