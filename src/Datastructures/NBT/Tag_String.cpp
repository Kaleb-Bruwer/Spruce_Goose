#include "Tag_String.h"

#include "../../Protocol/ProtocolHelpers.h" //stringFromBuffer()

using namespace std;

Tag_String::Tag_String(string val){
    typeID = 8;
    value = val;
}

Tag_String::Tag_String(char* iterator, int &index, int bufferSize, bool hasName)
        : NBT(hasName){
    typeID = 8;
    if(hasName)
        readRawName(iterator, index, bufferSize);
    value = readShortString(iterator, index, bufferSize);
}

Tag_String::Tag_String(Tag_String* rhs){
    *this = *rhs;
}

NBT* Tag_String::clone(){
    Tag_String* result = new Tag_String();
    *result = *this;
    return result;
}

Tag_String& Tag_String::operator=(const Tag_String& rhs){
    NBT::operator=(rhs);
    value = rhs.value;
    return *this;
}

string Tag_String::readShortString(char* iterator, int &index, int bufferSize){
    //Short int-prepended string
    int len = readShort(iterator, index, bufferSize);

    if(index + len > bufferSize)
        throw 0; //Can't read full string

    char save = iterator[len+index];
    iterator[len+index] = 0;
    string n(&iterator[index], len);
    iterator[len+index] = save;
    index += len;

    return n;
}

string Tag_String::printableStr(unsigned int nTabs){
    string result = "String("+name+"): " + value + "\n";
    for(int i=0; i<nTabs; i++){
        result = "\t" + result;
    }
    return result;
}

int Tag_String::getTransmitSize(){
    return (sizeNoData() + 2 + value.length());
}

void Tag_String::getTransmittable(char* buffer, int &index){
    //TypeID
    if(hasName){
        buffer[index] = (char)8;
        index++;
        writeName(buffer, index);
    }

    unsigned short int valLen = value.length();
    void* a = &valLen;
    memcpy(&(buffer[index]), a, 2);
    switchEndian(&(buffer[index]),2);
    index+=2;

    for(char c: value){
        buffer[index] = c;
        index++;
    }

}
