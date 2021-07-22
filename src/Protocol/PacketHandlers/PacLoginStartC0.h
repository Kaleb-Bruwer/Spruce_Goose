#ifndef PACLOGINSTARTC0_H
#define PACLOGINSTARTC0_H

#include "PacketHandlerRead.h"

class PacLoginStartC0 : public PacketHandlerRead{
public:
    PacLoginStartC0(PlayerConnection* p): PacketHandlerRead(p){};
    void handle(PacketReader &p){
        owner->username = p.readString();
        owner->loginRoutine();
    }
};

#endif
