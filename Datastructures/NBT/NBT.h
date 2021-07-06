#ifndef NBT_H
#define NBT_H

#include <string>
#include <cstring>

using namespace std;

class NBT{
protected:
    int typeID;
    bool hasName = true;
    string name;
    const bool bigEndian = false; //Not properly used
    void readRawName(char* iterator, int &index);
    void writeName(char* buffer, int &index);
    void switchEndian(void* iterator, int size);
    short int readShort(char* iterator, int &index);
    int sizeNoData();
public:
    NBT(bool hN = true){hasName = hN;};
    virtual ~NBT(){};
    virtual NBT* clone() = 0;
    NBT&  operator=(const NBT& rhs);

    void setHasName(bool newVal){hasName = newVal;};

    string getName(){return name;};
    void setName(string newName){name = newName; hasName = true;};

    virtual int getTypeID(){return typeID;};
    virtual string printableStr(unsigned int nTabs) = 0; //For debugging
    virtual int getTransmitSize() = 0;
    virtual void getTransmittable(char* buffer, int &index) = 0;
};

#endif
