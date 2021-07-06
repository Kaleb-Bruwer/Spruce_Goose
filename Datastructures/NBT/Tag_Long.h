#ifndef TAG_LONG_H
#define TAG_LONG_H

#include "NBT_Primitive.h"

using namespace std;

class Tag_Long : public NBT_Primitive<long long>{
public:
    Tag_Long(){typeID = 4;};
    Tag_Long(long long val);
    Tag_Long(char* iterator, int &index, bool hasName = true);
    Tag_Long(Tag_Long* rhs);
    NBT* clone();
    string printableStr(unsigned int nTabs = 0);
};

#endif
