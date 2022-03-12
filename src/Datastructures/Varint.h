#pragma once

#include <limits.h>
#include <string>

class Varint{
private:
    int nBytes = 0;
    unsigned long long value = 0;
    char* charVersion = 0; //null terminated for string's assignment operator
public:
    Varint(long long value);
    Varint(char* start, int &size, int sizeLim = INT_MAX); //throws 0 if sizeLim is exceeded
    Varint(const Varint &rhs);
    ~Varint();

    Varint& operator=(const Varint &rhs);

    unsigned long long getInt() {return value;};
    char* getVarint(){return charVersion;};
    void writeToBuffer(char* buffer, int &index);
    std::string getVarintAsString();
    int getNBytes(){return nBytes;}; //Length of Varint version in bytes
};
