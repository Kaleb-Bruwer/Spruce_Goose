#include "SendPacket.h"

#include <cstring>

#include "../Protocol/PacketWriter.h"

SendPacket::SendPacket(PacketWriter *p){
    size = p->getIndex();
    data = new char[size];
    char* b = p->getBuffer();

    memcpy(data, b, size);
}
