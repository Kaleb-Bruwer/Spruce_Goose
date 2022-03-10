#ifndef TAG_INT_ARRAY_H
#define TAG_INT_ARRAY_H

#include "NBT_Array.h"

using namespace std;

class Tag_Int_Array : public NBT_Array<int>{
public:
    Tag_Int_Array(){typeID = 11;};
    Tag_Int_Array(char* iterator, int &index, int bufferSize, bool hasName = true);
    Tag_Int_Array(Tag_Int_Array* rhs);
    NBT* clone();

    string printableStr(unsigned int nTabs = 0);
};

#endif
