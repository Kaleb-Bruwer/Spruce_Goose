#include "ProtocolHelpers.h"

#include <cstring>
#include <iostream>

using namespace std;

unsigned int varintToInt(char* nextChar, int& size, int sizeLim){
    //If this function returns UINT_MAX, it couldn't read whole value
    if(sizeLim != INT_MAX)
        sizeLim += size;
    nextChar += size;

    unsigned int result = 0;
    int currOffset = 0;
    bool flag = true;
    nextChar--;

    while(flag){
        nextChar++;
        size++;
        unsigned int curr = (*nextChar) & 0x7f;
        curr = curr << currOffset;
        result = result | curr;
        currOffset+= 7;

        if(!((*nextChar) & 0x80))
            return result;
        if(currOffset > 25 || size == sizeLim)
            return UINT_MAX;
    }
    return result;
}

unsigned long varlongToLong(char* nextChar, int& size, long long sizeLim){
    //If this function returns UINT_MAX, it couldn't read whole value

    nextChar += size;

    unsigned long result = 0;
    int currOffset = 0;
    size = 0;
    bool flag = true;
    nextChar--;

    while(flag){
        nextChar++;
        size++;
        unsigned int curr = (*nextChar) & 0x7f;
        curr = curr << currOffset;
        result = result | curr;
        currOffset+= 7;

        if(!((*nextChar) & 0x80))
            return result;
        if(currOffset > 57 || size == sizeLim)
            return UINT_MAX;
    }
    return result;
}

char* intToVarint(int val){
  //TODO: do this right
  if(val > 127)
    val = 127;
  char* result = new char[1];
  result[0] = val;
  return result;
}

string stringFromBuffer(char* buffer, int &index){
    int len = varintToInt(buffer, index);
    char save = buffer[len+index];
    buffer[len+index] = 0;
    string result(&buffer[index]);
    buffer[len+index] = save;
    index += len;
    return result;
}

string lenPrependedString(string in){
    string out = intToVarint(in.length()) +in;
    return out;
}

void switchEndian(void* iterator, int size){
    if(size > 64){
        char* temp = new char[size];
        for(int i = 0; i<size; i++){
            temp[size-i-1] = ((char*)iterator)[i];
        }
        memcpy(iterator,temp, size);
        delete [] temp;
    }
    else{
        char buffer[64];
        for(int i=0; i<size; i++){
            buffer[size - i - 1] = ((char*)iterator)[i];
        }
        memcpy(iterator, &(buffer[0]), size);
    }
}
