#ifndef TAG_BYTE_H
#define TAG_BYTE_H

#include "NBT_Primitive.h"

using namespace std;

class Tag_Byte : public NBT_Primitive<char>{
public:
    Tag_Byte(){typeID = 1;};
    Tag_Byte(char val);
    Tag_Byte(char* iterator, int &index, bool hasName = true);//TypeID excluded
    Tag_Byte(Tag_Byte* rhs);
    NBT* clone();
    int getTypeID(){return 1;};
    string printableStr(unsigned int nTabs = 0);
};

#endif
