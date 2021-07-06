#ifndef PACPOSITIONANDLOOKS34_H
#define PACPOSITIONANDLOOKS34_H

#include "PacketHandlerWrite.h"

#include "../../JobTickets/WorldToProtocol/SendPlayerPos.h"

class PacPositionAndLookS34 : public PacketHandlerWrite{
public:
    PacPositionAndLookS34(PlayerConnection* p): PacketHandlerWrite(p){};

    SendPlayerPos* indirectParam = 0; //Just a reference, not owned

    void handle(){
        if(!indirectParam)
            return;

        PacketWriter packet;

        Varint packetID(0x34);
        packet << packetID;

        Coordinate<double> pos = indirectParam->pos;

        packet << pos.x;
        packet << pos.y;
        packet << pos.z;

        packet << indirectParam->yaw;
        packet << indirectParam->pitch;

        //States if value is relative or not
        char flag = 0;
        packet << flag;

        //TeleportID (id client should confirm with)
        Varint teleportID(101);
        packet << teleportID;

        packet.addMsgLen();
        sendMessage(packet);
    };
};

#endif
