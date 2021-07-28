#include "PacketReader.h"

#include <cstring>
#include <iostream>

#include "ProtocolHelpers.h"

using namespace std;

PacketReader::PacketReader(char* start, int s){
    //MUST copy data to local buffer, start may be on the stack
    size = s;
    bufferSize = s;

    buffer = new char[bufferSize];
    memcpy(buffer, start, size);

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
    if(buffer)
        delete [] buffer;
}

int PacketReader::append(char* start, int len){
    if((index + bufferSize - size) > len){
        //Buffer has space if already-used data is removed
        int dist = 0 - index;

        memmove(&(buffer[0]), &buffer[index], size - index);
        memcpy(&buffer[size - index], start, len);
        index = 0;
        size = size - index + len;
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

bool PacketReader::reachedEnd(){
    return (index >= size);
}

short PacketReader::readShort(){
    short result;
    memcpy(&result, &(buffer[index]),2);
    index += 2;
    switchEndian(&result, 2);
    return result;
}

unsigned short PacketReader::readUShort(){
    unsigned short result;
    memcpy(&result, &(buffer[index]),2);
    switchEndian(&result, 2);
    index +=2;
    return result;
}

char PacketReader::readChar(){
    index++;
    return buffer[index -1];
}

unsigned char PacketReader::readUChar(){
    index++;
    return buffer[index -1];
}

bool PacketReader::readBool(){
    index++;
    return buffer[index-1];
}

Varint PacketReader::readVarint(){
    int oldIndex = index;
    Varint result(buffer, index);
    return result;
}

string PacketReader::readString(){
    Varint len(buffer, index);
    string result(&(buffer[index]), len.getInt());
    index += len.getInt();
    return result;
}

int PacketReader::readInt(){
    int result;
    memcpy(&result, &(buffer[index]),4);
    switchEndian(&result, 4);
    index +=4;
    return result;
}

long long PacketReader::readLongLong(){
    long long result;
    memcpy(&result, &(buffer[index]),8);
    switchEndian(&result, 8);
    index +=8;
    return result;
}

float PacketReader::readFloat(){
    float result;
    memcpy(&result, &(buffer[index]),4);
    switchEndian(&result, 4);
    index +=4;
    return result;
}

double PacketReader::readDouble(){
    double result;
    memcpy(&result, &(buffer[index]),8);
    switchEndian(&result, 8);
    index +=8;
    return result;
}

Tag_Compound* PacketReader::readNBT(){
    int oldIndex = index;
    Tag_Compound* result = new Tag_Compound(buffer, index, false);
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
        cout << "Can't read segment\n";
        return;
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
