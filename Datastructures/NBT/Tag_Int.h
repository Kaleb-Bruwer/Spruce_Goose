#ifndef TAG_INT_H
#define TAG_INT_H

#include "NBT_Primitive.h"

using namespace std;

class Tag_Int : public NBT_Primitive<int>{
public:
    Tag_Int(){typeID = 3;};
    Tag_Int(int val);
    Tag_Int(char* iterator, int &index, bool hasName = true);
    Tag_Int(Tag_Int* rhs);
    NBT* clone();
    string printableStr(unsigned int nTabs = 0);
};

#endif
