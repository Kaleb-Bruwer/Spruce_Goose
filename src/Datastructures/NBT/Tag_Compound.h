#ifndef TAG_COMPOUND_H
#define TAG_COMPOUND_H

#include <vector>
#include <limits.h>

#include "NBT.h"

using namespace std;

class Tag_Compound : public NBT{
private:
    vector<NBT*> children;

    void addItem(char* iterator, int &index, unsigned short int type, int bufferSize);
public:
    Tag_Compound();

    // throws (int) 0 if buffer limit must be exceeded
    // To prevent memory leaks, root node must still be deleted.
    Tag_Compound(char* start, int &index, int bufferSize = INT_MAX, bool hasName = true);
    Tag_Compound(Tag_Compound* rhs);
    ~Tag_Compound();

    NBT* clone(); //Deep copy
    Tag_Compound& operator=(const Tag_Compound& rhs);//Deep copy

    int getTypeID(){return 10;};
    string printableStr(unsigned int nTabs = 0);

    void addItem(NBT* item); //Shallow copy
    NBT* getItem(string name);

    int getTransmitSize(); //Add 1 to result
    void getTransmittable(char* buffer, int &index);
};

#endif
