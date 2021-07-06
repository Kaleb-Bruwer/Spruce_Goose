#ifndef PACKETHANDLERREAD_H
#define PACKETHANDLERREAD_H

#include "../PlayerConnection.h"

using namespace std;

class PacketHandlerRead{
protected:
    PlayerConnection* owner = 0;//Just a refernece, obviously not owned
    void sendMessage(PacketWriter* p){
        owner->sendMessage(p);
    };
    void pushJobToThreadArea(JobTicket* job);
    int getEID(){
        return owner->eid;
    };

public:
    PacketHandlerRead(PlayerConnection* o){
        owner = o;
    };
    virtual void handle(PacketReader &r) = 0;
};

#endif
