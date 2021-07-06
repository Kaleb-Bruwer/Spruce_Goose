#include "PacketWriter.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

#include <unistd.h>

#include "ProtocolHelpers.h"
#include "../zlib/zlib.h"

using namespace std;

PacketWriter::PacketWriter(unsigned int startSize){
    size = startSize;
    buffer = new char[size];
}

PacketWriter::~PacketWriter(){
    if(buffer)
        delete [] buffer;
    buffer = 0;
}

void PacketWriter::sendPacket(int sock){
    if(buffer && index > 0)
        write(sock, buffer, index);
}

void PacketWriter::resizeBuffer(){
    int newSize = size * resizeFactor;
    char* temp = new char[newSize];
    memcpy(temp, buffer, index);
    delete [] buffer;
    buffer = temp;
    size = newSize;
}

void PacketWriter::resizeBuffer(int newSize){
    int defaultSize = size * resizeFactor;

    //The +8 is to be on the safe side
    while(defaultSize < newSize + 8){
        defaultSize *= resizeFactor;
    }
    newSize = defaultSize;

    char* temp = new char[newSize];
    memcpy(temp, buffer, index);
    delete [] buffer;
    buffer = temp;
    size = newSize;
}

void PacketWriter::print(){
    cout << '\n';
    for(int i=0; i< index; i++){
        cout << setw(2) << hex << setfill('0') << (int)((unsigned char)(buffer[i])) << ' ';
    }
    cout << dec << endl;
}

void PacketWriter::writeEndianSwitched(void* data, unsigned int size){
    switchEndian(data, size);
    writeVoidPtr(data, size);
}

void PacketWriter::addMsgLen(){
    int lenVal = index;

    Varint len(lenVal);

    int lenlen = len.getNBytes();
    int newSize = index + lenlen;
    if(newSize >= size){
        resizeBuffer(newSize); //TODO: more efficient by adding varint to new buffer first
    }
    memmove(&(buffer[lenlen]), &(buffer[0]), index);
    index += lenlen;
    int temp = 0;
    len.writeToBuffer(buffer, temp);
}

int PacketWriter::compressAll(){
    uLong rawSize = index;
    uLong compSize = compressBound(rawSize);
    char* newBuffer = new char[compSize];

    size = compSize;

    compress((Bytef*)newBuffer, &compSize, (Bytef*)buffer, rawSize);

    delete [] buffer;
    buffer = newBuffer;
    index = compSize;
    return index;
}

void PacketWriter::writePacketID(int val){
    (*this) << Varint(val);
}

void PacketWriter::writeArr(unsigned char* b, int l){
    writeVoidPtr((void*) b, l);
}

void operator<<(PacketWriter &p, short val){
    p.writeEndianSwitched((void*)&val, 2);
}

void operator<<(PacketWriter &p, unsigned short val){
    p.writeEndianSwitched((void*)&val, 2);
}

void operator<<(PacketWriter &p, char val){
    p.writeVoidPtr((void*) &val, 1);
}

void operator<<(PacketWriter &p, unsigned char val){
    p.writeVoidPtr((void*) &val, 1);
}

void operator<<(PacketWriter &p, bool val){
    p.writeVoidPtr((void*) &val, 1);
}

void operator<<(PacketWriter &p, Varint val){
    int index = p.getIndex();
    val.writeToBuffer(p.getBuffer(), index);
    p.setIndex(index);
}

void PacketWriter::writeUUID(char* start){
    writeVoidPtr((void*)start, 16);
}

void operator<<(PacketWriter &p, string s){
    Varint size(s.length());
    p << size;
    void* a = (void*) s.c_str();
    p.writeVoidPtr(a, size.getInt());
}

void operator<<(PacketWriter &p, int val){
    p.writeEndianSwitched((void*)&val, 4);
}

void operator<<(PacketWriter &p, long long val){
    p.writeEndianSwitched((void*)&val, 8);
}

void operator<<(PacketWriter &p, float val){
    p.writeEndianSwitched((void*)&val, 4);
}

void operator<<(PacketWriter &p, double val){
    p.writeEndianSwitched((void*)&val, 8);
}

void operator<<(PacketWriter &p, Tag_Compound &tag){
    int tagSize = tag.getTransmitSize();
    if(tagSize + p.index >= p.size)
        p.resizeBuffer(tagSize + p.index);
    int index = p.getIndex();
    tag.getTransmittable(p.getBuffer(), index);
    p.setIndex(index);
}

void operator<<(PacketWriter &p, Coordinate<int> coord){
    long long pos = (coord.x&0x3FFFFFF) << 38;
    pos |= (coord.z & 0x3FFFFFF) << 12;
    pos |= coord.y & 0xFFF;
    p << pos;
}

void operator<<(PacketWriter &p, EntityMetadata &val){
    for(EntityMetaValBase* e : val.data){
        unsigned char descriptor = e->getDescriptor();
        p << descriptor;

        switch(e->type){
        case 0:
        {
            EntityMetaVal<char>* child = (EntityMetaVal<char>*)e;
            p << child->data;
            break;
        }

        case 1:
        {
            EntityMetaVal<short>* child = (EntityMetaVal<short>*)e;
            p << child->data;
            break;
        }

        case 2:
        {
            EntityMetaVal<int>* child = (EntityMetaVal<int>*)e;
            p << child->data;
            break;
        }

        case 3:
        {
            EntityMetaVal<float>* child = (EntityMetaVal<float>*)e;
            p << child->data;
            break;
        }

        case 4:
        {
            EntityMetaVal<string>* child = (EntityMetaVal<string>*)e;
            p << child->data;
            break;
        }

        case 5:
        {
            EntityMetaVal<Slot*>* child = (EntityMetaVal<Slot*>*)e;
            p << *child->data;
            break;
        }

        case 6:
        {
            EntityMetaVal<Coordinate<int>>* child = (EntityMetaVal<Coordinate<int>>*)e;
            p << child->data;
            break;
        }

        }
    }
    p << (unsigned char)127;
}

void operator<<(PacketWriter &p, Slot s){
    if(s.itemID == 0 || s.itemCount == 0){
        p << (short)-1;
    }
    else{
        p << (short) s.itemID;
        p << (char) s.itemCount;
        p << (short) s.itemDamage;

        short tagSize = -1;
        if(s.nbtData){
            tagSize = s.nbtData->getTransmitSize();
        }
        p << tagSize;

        if(s.nbtData){
            //UNTESTED
            p << (Tag_Compound*) s.nbtData;
        }
    }
}

void operator<<(PacketWriter &lhs, PacketWriter &rhs){
    int newSize = lhs.index + rhs.index;
    if(newSize >= lhs.size)
        lhs.resizeBuffer(newSize);
    memcpy(&(lhs.buffer[lhs.index]), rhs.buffer, rhs.index);
    lhs.index += rhs.index;
}
