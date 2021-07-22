#ifndef PACINVENTORYS13_H
#define PACINVENTORYS13_H

#include "PacketHandlerWrite.h"

using namespace std;

class PacInventoryS13 : public PacketHandlerWrite{
    PacInventoryS13(PlayerConnection* p): PacketHandler(p){};

    void handle(){
        PacketWriter packet;

        Varint packetID(0x13);
        packet << packetID;

        char windowID = 0;
        packet << windowID;
        short count = 44;
        packet << count;

        for(int i=0; i<44; i++){
            //Temporary, sends an empty inventory
            bool present = false;
            packet <<present;
        }

        packet.addMsgLen();
        owner->sendMessage(packet);
    };
};

#endif
