#ifndef TAG_LIST_H
#define TAG_LIST_H

#include "NBT.h"

using namespace std;

class Tag_List : public NBT{
private:
    int type = 0;
    unsigned int currSize = 0;
    unsigned int size = 0;
    NBT** value = 0;

    void addItem(char* iterator, int &index, int bufferSize, int pos);
public:
    Tag_List(){typeID = 9;};
    Tag_List(int type, int size);
    Tag_List(char* iterator, int &index, int bufferSize, bool hasName = true);
    Tag_List(Tag_List* rhs);
    ~Tag_List();
    NBT* clone();
    Tag_List& operator=(const Tag_List& rhs);


    int getTypeID(){return 9;};
    NBT** getVal(){return value;};
    NBT* getValAt(int index);
    int getType(){return type;};
    int getSize(){return size;};
    string printableStr(unsigned int nTabs = 0);

    void addItem(NBT* item); //Shallow copy

    int getTransmitSize();
    void getTransmittable(char* buffer, int &index);

};

#endif
