#include "Varint.h"

#include <cmath>
#include <cstring>

using namespace std;

Varint::Varint(long long value){
    this->value = value;

    nBytes = ceil(log2(value+1)/7); //+1 since 128 needs 2 bytes)
    if(nBytes == 0)
        nBytes = 1;

    charVersion = new char[nBytes+1];

    charVersion[nBytes] = (char)0;

    for(int i=0; i < nBytes -1; i++){
        charVersion[i] = (char)((value % 128)+128);
        value = (value >> 7);
    }
    charVersion[nBytes-1] = (char)(value%128);
}

Varint::~Varint(){
    if(charVersion)
        delete [] charVersion;
}

Varint::Varint(char* nextChar, int&index, int sizeLim){
    if(sizeLim != INT_MAX)
        sizeLim += index;
    nextChar += index;
    char* start = nextChar;

    nBytes = 1;
    while(*nextChar < 0){
        nBytes++;
        nextChar++;
    }
    nextChar = start;

    if(nBytes > sizeLim){ //Value too big
        value = ULLONG_MAX;
        return;
    }

    charVersion = new char[nBytes+1];
    int currOffset = 0;
    for(int i=0; i<nBytes; i++){
        charVersion[i] = *nextChar;
        unsigned long long byteVal = ((unsigned int)(*nextChar) % 128);
        nextChar++;

        byteVal = (byteVal << currOffset);
        currOffset += 7;
        value = value | byteVal;
    }
    index += nBytes;

}

Varint::Varint(const Varint &rhs){
    *this = rhs;
}

Varint& Varint::operator=(const Varint &rhs){
    value = rhs.value;
    nBytes = rhs.nBytes;
    if(rhs.charVersion){
        charVersion = new char[nBytes+1];
        memcpy(charVersion, rhs.charVersion, nBytes+1);
    }
}


void Varint::writeToBuffer(char* buffer, int &index){
    memcpy(&(buffer[index]), charVersion, nBytes);
    index += nBytes;
}

string Varint::getVarintAsString(){
    string result(charVersion, nBytes);
    return result;
}
