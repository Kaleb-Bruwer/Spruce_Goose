#ifndef PACKETREADER_H
#define PACKETREADER_H

#include <iostream>
#include <gtest/gtest.h>

#include "../Datastructures/Varint.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/NBT/Tag_Compound.h"
#include "../Inventory/Slot.h"

using namespace std;

//There used to be a PacketReaderOld that inherited from this class
//in order to read 1.4 packets. Its compatibility has been stripped out
//since it is no longer needed

class MapChunkBulkReader;

class PacketReader{
protected:
    int size; //useful data
    int bufferSize; //actual buffer
    int index = 0;
    char* buffer = 0;

    friend class MapChunkBulkReader;
    FRIEND_TEST(PacketReader, append);
    PacketReader(){};

public:
    //Will automatically read length from start of packet
    PacketReader(char* start, int size);
    virtual ~PacketReader();

    void append(char* start, int size);

    bool reachedEnd();

    int lenRemaining(){
        return size - index;
    }

    int getSize(){
        return size;
    };
    int getIndex(){
        return index;
    };
    void setIndex(int a){
        index = a;
    }
    void skip(int numBytes){
        index += numBytes;
    }

    int readPacketID(){return readVarint().getInt();};
    short readShort();
    unsigned short readUShort();
    char readChar();
    unsigned char readUChar();
    bool readBool();
    Varint readVarint();
    string readString();
    int readInt();
    long long readLongLong();
    float readFloat();
    double readDouble();
    Tag_Compound* readNBT();
    Coordinate<int> readPosition();
    Slot readSlot();

    // Copies from buffer to start and moves buffer's index
    void readSegment(unsigned short numBytes, char* start);

    friend ostream& operator<<(ostream& out, const PacketReader& obj);
};

#endif
