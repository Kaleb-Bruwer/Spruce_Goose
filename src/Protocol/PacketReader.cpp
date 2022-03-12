#include "PacketReader.h"

#include <cstring>
#include <iostream>

#include "ProtocolHelpers.h"

using namespace std;

PacketReader::PacketReader(unsigned int bufferSize){
    size = 0;
    buffer = new char[bufferSize];
    bufferSize = bufferSize;
}

PacketReader::PacketReader(char* start, int s){
    //MUST copy data to local buffer, start may be on the stack
    size = s;
    bufferSize = s;

    if(s > 0){
        buffer = new char[bufferSize];
        memcpy(buffer, start, size);
    }
    else{
        buffer = new char[1024];
        bufferSize = 1024;
    }

    /*
    Varint len(start, index);//Moves index on
    size = len.getInt();
    rawSize = size + len.getNBytes();

    buffer = new char[rawSize];
    memcpy(buffer, &(start[indexExternal]), rawSize);
    indexExternal += rawSize;
    */
}

PacketReader::~PacketReader(){
    if(buffer && ownsBuffer)
        delete [] buffer;
}

int PacketReader::append(PacketReader &read){
    return append(read.buffer, read.size);
}


int PacketReader::append(char* start, int len){
    if((index + bufferSize - size) > len){
        //Buffer has space if already-used data is removed
        int dist = 0 - index;

        memmove(&(buffer[0]), &buffer[index], size - index);
        memcpy(&buffer[size - index], start, len);
        size = size - index + len;
        index = 0;
        return dist;
    }
    else{
        int oldSize = size - index;
        char* newBuffer = new char[oldSize + len];
        memcpy(&(newBuffer[0]), &(buffer[index]), oldSize);
        memcpy(&(newBuffer[oldSize]), start, len);

        delete [] buffer;
        buffer = newBuffer;

        index = 0;
        bufferSize = oldSize + len;
        size = bufferSize;
        return 0;
    }
}

void PacketReader::swapGuts(PacketReader &rhs){
    int temp;

    temp = size;
    size = rhs.size;
    rhs.size = temp;

    temp = bufferSize;
    bufferSize = rhs.bufferSize;
    rhs.bufferSize = temp;

    temp = index;
    index = rhs.index;
    rhs.index = temp;

    char* b = buffer;
    buffer = rhs.buffer;
    rhs.buffer = b;
}


bool PacketReader::reachedEnd(){
    return (index >= size);
}

// short PacketReader::readShort(){
//     short result;
//     memcpy(&result, &(buffer[index]),2);
//     index += 2;
//     switchEndian(&result, 2);
//     return result;
// }

// unsigned short PacketReader::readUShort(){
//     unsigned short result;
//     memcpy(&result, &(buffer[index]),2);
//     switchEndian(&result, 2);
//     index +=2;
//     return result;
// }
//
// char PacketReader::readChar(){
//     index++;
//     return buffer[index -1];
// }
//
// unsigned char PacketReader::readUChar(){
//     index++;
//     return buffer[index -1];
// }
//
// bool PacketReader::readBool(){
//     index++;
//     return buffer[index-1];
// }

Varint PacketReader::readVarint(){
    int oldIndex = index;
    Varint result(buffer, index, bufferSize - index);
    return result;
}

string PacketReader::readString(){
    Varint len(buffer, index, bufferSize - index);
    if(len.getInt() + index > bufferSize){
        throw 0;
    }
    string result(&(buffer[index]), len.getInt());
    index += len.getInt();
    return result;
}

// int PacketReader::readInt(){
//     int result;
//     memcpy(&result, &(buffer[index]),4);
//     switchEndian(&result, 4);
//     index +=4;
//     return result;
// }
//
// long long PacketReader::readLongLong(){
//     long long result;
//     memcpy(&result, &(buffer[index]),8);
//     switchEndian(&result, 8);
//     index +=8;
//     return result;
// }
//
// float PacketReader::readFloat(){
//     float result;
//     memcpy(&result, &(buffer[index]),4);
//     switchEndian(&result, 4);
//     index +=4;
//     return result;
// }
//
// double PacketReader::readDouble(){
//     double result;
//     memcpy(&result, &(buffer[index]),8);
//     switchEndian(&result, 8);
//     index +=8;
//     return result;
// }

Tag_Compound* PacketReader::readNBT(){
    int oldIndex = index;
    Tag_Compound* result = 0;
    try{
        result = new Tag_Compound(buffer, index, bufferSize, false);
    }
    catch(int e){
        if(result)
            delete result;
        throw e;
    }
    return result;
}

Coordinate<int> PacketReader::readPosition(){
    long long position = readLongLong();
    Coordinate<int> result;
    result.x = position >> 38;
    result.y = position & 0xFFF;
    result.z = (position << 26 >> 38);
    return result;
}

Slot PacketReader::readSlot(){
    Slot result;
    result.itemID = readShort();
    if(result.itemID == -1)
        return result;

    result.itemCount = readChar();
    result.itemDamage = readShort();
    short len = readShort(); //Length of nbt data to follow
    if(len > 0){
        result.nbtData = readNBT();
    }
    return result;
}

void PacketReader::readSegment(unsigned short numBytes, char* start){
    if(index + numBytes > size){
        throw 0;
    }

    memcpy(start, &(buffer[index]), numBytes);
    index += numBytes;
}


ostream& operator<<(ostream& out, const PacketReader& obj){
    if(obj.size < 128){
        for(int i=0; i<obj.size; i++){
            out << (int) (unsigned char) obj.buffer[i] << " ";
        }
    }

    return out;
}
