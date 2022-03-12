#ifndef PACKETWRITEROLDMOTD_H
#define PACKETWRITEROLDMOTD_H

#include "PacketWriterOld.h"

using namespace std;

//HOW TO USE:
//writePacketID(0xFF)
//nullSeparator (used as placeholder for length)
//weirdSeparator (adds required but useless characters)
//writer.nullSeparator();
/*
All of the following values are strings
writer << prVersion;
writer.nullSeparator();
writer << mcVersion;
writer.nullSeparator();
writer << motd;
writer.nullSeparator();
writer << playerCount;
writer.nullSeparator();
writer << playerCap;
writer.writeLenInGap();
*/

class PacketWriterOldMOTD : public PacketWriterOld{
public:
    PacketWriterOldMOTD(): PacketWriterOld(){};

    void writeLenInGap(short val = 0);

    void weirdSeparator(); //Adds the (00 A7 00 31) at the start of the packet
    void nullSeparator(); //Adds 00 00

    friend void operator<<(PacketWriterOldMOTD &p, unsigned char val);
    friend void operator<<(PacketWriterOldMOTD &p, char val);

    friend void operator<<(PacketWriterOldMOTD &p, short val);
    friend void operator<<(PacketWriterOldMOTD &p, string val);
};

#endif
