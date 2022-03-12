#ifndef TAG_DOUBLE_H
#define TAG_DOUBLE_H

#include "NBT_Primitive.h"

using namespace std;

class Tag_Double : public NBT_Primitive<double>{
public:
    Tag_Double(){typeID = 6;};
    Tag_Double(double val);
    Tag_Double(char* iterator, int &index, int bufferSize, bool hasName = true);
    Tag_Double(Tag_Double* rhs);
    NBT* clone();
    string printableStr(unsigned int nTabs = 0);

};

#endif
