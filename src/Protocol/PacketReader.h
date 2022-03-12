#ifndef PACKETREADER_H
#define PACKETREADER_H

#include <iostream>
#include <gtest/gtest.h>

#include "ReadFromBuffer.h"

#include "../Datastructures/Varint.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/NBT/Tag_Compound.h"
#include "../Inventory/Slot.h"

using namespace std;

//There used to be a PacketReaderOld that inherited from this class
//in order to read 1.4 packets. Its compatibility has been stripped out
//since it is no longer needed

// There is a child class called PacketReaderSub whose only difference is that it
// doesn't delete the buffer. This is for creating a PacketReader per packet while
// the PacketReader object holds multiple

class MapChunkBulkReader;

class PacketReader{
protected:
    bool ownsBuffer = true;

    int size; //useful data
    int bufferSize; //actual buffer
    int index = 0;
    char* buffer = 0;

    friend class MapChunkBulkReader;
    friend class PacketReaderSub;
    FRIEND_TEST(PacketReader, append);
    PacketReader(){};

public:
    PacketReader(char* start, int size);
    PacketReader(unsigned int bufferSize); //No data, but has a buffer ready
    virtual ~PacketReader();

    // Returns change to index, either 0 or negative (moved to left)
    int append(char* start, int size);
    int append(PacketReader &rhs);
    void swapGuts(PacketReader &rhs);

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
    void setIndex(int a){ //enough rope to hang yourself, be careful
        index = a;
    }
    void skip(int numBytes){
        if(index + numBytes > size){
            throw 0;
        }
        index += numBytes;
    }

    short readShort(){return ReadFromBuffer::read<short>(buffer, index, size);};
    unsigned short readUShort(){return ReadFromBuffer::read<unsigned short>(buffer, index, size);};
    char readChar(){return ReadFromBuffer::read<char>(buffer, index, size);};
    unsigned char readUChar(){return ReadFromBuffer::read<unsigned char>(buffer, index, size);};
    bool readBool(){return ReadFromBuffer::read<bool>(buffer, index, size);};
    int readInt(){return ReadFromBuffer::read<int>(buffer, index, size);};
    long long readLongLong(){return ReadFromBuffer::read<long long>(buffer, index, size);};
    float readFloat(){return ReadFromBuffer::read<float>(buffer, index, size);};
    double readDouble(){return ReadFromBuffer::read<double>(buffer, index, size);};

    // special cases
    int readPacketID(){return readVarint().getInt();};
    Varint readVarint();
    string readString();
    Tag_Compound* readNBT();
    Coordinate<int> readPosition();
    Slot readSlot();

    // Copies from buffer to start and moves buffer's index
    void readSegment(unsigned short numBytes, char* start);

    friend ostream& operator<<(ostream& out, const PacketReader& obj);
};

#endif
