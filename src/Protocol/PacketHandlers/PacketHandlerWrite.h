#ifndef PACKETHANDLERWRITE_H
#define PACKETHANDLERWRITE_H

#include "../PlayerConnection.h"

using namespace std;

class PacketHandlerWrite{
protected:
    PlayerConnection* owner = 0;//Just a refernece, obviously not owned
    void genRandomUUID(){
        owner->genRandomUUID();
    };
    void sendMessage(PacketWriter& p){
        owner->sendMessage(p);
    }

public:
    PacketHandlerWrite(PlayerConnection* o){
        owner = o;
    };
    virtual void handle() = 0;
};

#endif
