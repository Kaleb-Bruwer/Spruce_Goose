#include "NBT.h"

#include <cstring>

#include "../Varint.h"
#include "../../Protocol/ReadFromBuffer.h"

using namespace std;

NBT& NBT::operator=(const NBT& rhs){
    typeID = rhs.typeID;
    hasName = rhs.hasName;
    name = rhs.name;
}

void NBT::readRawName(char* iterator, int &index, int bufferSize){
    int len = readShort(iterator, index, bufferSize);

    if(index + len > bufferSize)
        throw 0;

    char save = iterator[len+index];
    iterator[len+index] = 0;
    string n(&iterator[index]);
    name = n;
    iterator[len+index] = save;
    index += len;
}

void NBT::switchEndian(void* iterator, int size){
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

short int NBT::readShort(char* iterator, int &index, int bufferSize){
    return ReadFromBuffer::read<short>(iterator, index, bufferSize);
}

int NBT::sizeNoData(){
    if(hasName){
        //2 for name size prefix & 1 for TypeID
        return name.length() + 2;
    }
    else return 0;
}

void NBT::writeName(char* iterator, int &index){
    unsigned short int len = name.length();
    memcpy(&(iterator[index]), (void*)&len, 2);
    switchEndian(&(iterator[index]), 2);
    index+=2;
    for(char c: name){
        iterator[index] = c;
        index++;
    }
}
