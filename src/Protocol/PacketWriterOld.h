#ifndef PACKETWRITEROLD_H
#define PACKETWRITEROLD_H

#include "PacketWriter.h"

using namespace std;

class PacketWriterOld : public PacketWriter{
private:
    void writeEntityMetaVal(EntityMetaValBase* val);
public:
    PacketWriterOld(unsigned int size = 256): PacketWriter(size){};
    ~PacketWriterOld(){};

    //These manually call parent's operators since they can't be inherited
    //and I'm not gonna cast things around outside of here

    void writePacketID(int val);

    friend void operator<<(PacketWriterOld &p, short val);
    friend void operator<<(PacketWriterOld &p, unsigned short val);
    friend void operator<<(PacketWriterOld &p, char val);
    friend void operator<<(PacketWriterOld &p, unsigned char val);
    friend void operator<<(PacketWriterOld &p, bool val);
    //No size limits enforced by this method
    friend void operator<<(PacketWriterOld &p, Varint &val);
    friend void operator<<(PacketWriterOld &p, string &s);
    friend void operator<<(PacketWriterOld &p, int val);
    friend void operator<<(PacketWriterOld &p, long long val);
    friend void operator<<(PacketWriterOld &p, float val);
    friend void operator<<(PacketWriterOld &p, double val);
    friend void operator<<(PacketWriterOld &p, Tag_Compound &val);
    friend void operator<<(PacketWriterOld &p, Coordinate<int> &coord);
    friend void operator<<(PacketWriterOld &p, EntityMetadata &val);
    friend void operator<<(PacketWriterOld &p, Slot &s);

    friend void operator<<(PacketWriterOld &lhs, PacketWriterOld &rhs);
};

#endif
