#ifndef PACKETWRITER_H
#define PACKETWRITER_H

#include "../Datastructures/Varint.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Inventory/EntityMetadata.h"
#include "../Datastructures/NBT/Tag_Compound.h"

using namespace std;

//Allows child classes to use operators
#define baseThis *((PacketWriter*) this)

class PacketWriter{
private:
    int lastPacket = 0;
protected:
    const float resizeFactor = 2;
    int size;
    int index = 0;
    char* buffer = 0;

    //This functon is inline for performance reasons, since it's small,
    //but used very often
    void writeVoidPtr(void* data, unsigned int len){
        while(len + index >= size)
            resizeBuffer();
        memcpy(&(buffer[index]), data, len);
        index += len;
    };
    void writeEndianSwitched(void* ptr, unsigned int size);//Alters ptr's data
public:
    bool oldProtocol = false; //Changes some of the types

    PacketWriter(unsigned int size = 256);
    virtual ~PacketWriter();

    void sendPacket(int sock);

    int getSize(){return size;};//Not the content size, the actual buffer size
    int getIndex(){return index;}; //Not recommended to use this way
    void setIndex(int i){index = i;};
    void setOldProtocol(bool o){oldProtocol = o;};
    char* getBuffer(){return buffer;};
    void print();

    void resizeBuffer();
    void resizeBuffer(int newSize); //Uses newSize if greater than default val

    //Prepends message with varint containing message length
    void addMsgLen();

    //Compresses all the data in the buffer, no packet format stuff
    //returns compressed size
    int compressAll();

    //Rather use the << overloads to write
    virtual void writePacketID(int);
    void writeArr(unsigned char* start, int len);
    friend void operator<<(PacketWriter &p, short val);
    friend void operator<<(PacketWriter &p, unsigned short val);
    friend void operator<<(PacketWriter &p, char val);
    friend void operator<<(PacketWriter &p, unsigned char val);
    friend void operator<<(PacketWriter &p, bool val);
    //No size limits enforced by this method
    friend void operator<<(PacketWriter &p, Varint val);
    void writeUUID(char* start);
    friend void operator<<(PacketWriter &p, string s);
    friend void operator<<(PacketWriter &p, int val);
    friend void operator<<(PacketWriter &p, long long val);
    friend void operator<<(PacketWriter &p, float val);
    friend void operator<<(PacketWriter &p, double val);
    friend void operator<<(PacketWriter &p, Tag_Compound &val);
    friend void operator<<(PacketWriter &p, Coordinate<int> coord);
    friend void operator<<(PacketWriter &p, EntityMetadata &val);
    friend void operator<<(PacketWriter &p, Slot s);

    friend void operator<<(PacketWriter &lhs, PacketWriter &rhs);

};

#endif
