#ifndef TAG_SHORT_H
#define TAG_SHORT_H

#include "NBT_Primitive.h"

using namespace std;

class Tag_Short : public NBT_Primitive<short>{
public:
    Tag_Short(){typeID = 2;};
    Tag_Short(short int val);
    Tag_Short(char* iterator, int &index, bool hasName = true);//TypeID excluded
    Tag_Short(Tag_Short* rhs);
    NBT* clone();
    string printableStr(unsigned int nTabs = 0);
};

#endif
