#ifndef TAG_FLOAT_H
#define TAG_FLOAT_H

#include "NBT_Primitive.h"

using namespace std;

class Tag_Float : public NBT_Primitive<float>{
public:
    Tag_Float(){typeID = 5;};
    Tag_Float(float val);
    Tag_Float(char* iterator, int& index, bool hasName = true);
    Tag_Float(Tag_Float* rhs);
    NBT* clone();
    string printableStr(unsigned int nTabs = 0);
};

#endif
