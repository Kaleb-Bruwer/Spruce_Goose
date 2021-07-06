#ifndef TAG_COMPOUND_H
#define TAG_COMPOUND_H

#include <vector>

#include "NBT.h"

using namespace std;

class Tag_Compound : public NBT{
private:
    vector<NBT*> children;

    void addItem(char* iterator, int &index, unsigned short int type);
public:
    Tag_Compound();
    Tag_Compound(char* start, int &index, bool hasName = true);
    Tag_Compound(Tag_Compound* rhs);
    ~Tag_Compound();

    NBT* clone(); //Deep copy
    Tag_Compound& operator=(const Tag_Compound& rhs);//Deep copy

    int getTypeID(){return 10;};
    string printableStr(unsigned int nTabs = 0);

    void addItem(NBT* item); //Shallow copy
    NBT* getItem(string name);

    int getTransmitSize();
    void getTransmittable(char* buffer, int &index);
};

#endif
