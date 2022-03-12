#ifndef TAG_STRING_H
#define TAG_STRING_H

#include <string>

#include "NBT.h"

using namespace std;

class Tag_String : public NBT{
private:
    string value;

    //String prefixed with 2 byte length
    string readShortString(char* iterator, int& index, int bufferSize);
public:
    Tag_String(){typeID = 8;};
    Tag_String(string val);
    Tag_String(char* iterator, int &index, int bufferSize, bool hasName = true);
    Tag_String(Tag_String* rhs);
    NBT* clone();
    Tag_String& operator=(const Tag_String& rhs);

    int getTypeID(){return 8;};
    string getVal(){return value;};
    string printableStr(unsigned int nTabs = 0);

    int getTransmitSize();
    void getTransmittable(char* buffer, int &index);
};

#endif
