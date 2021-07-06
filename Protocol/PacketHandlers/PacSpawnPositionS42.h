#ifndef PACSPAWNPOSITIONS42_H
#define PACSPAWNPOSITIONS42_H

#include "PacketHandlerWrite.h"
#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class PacSpawnPositionS42 : public PacketHandlerWrite{
    PacSpawnPositionS42(PlayerConnection* p): PacketHandler(p){};

    void handle(){
        PacketWriter packet;

        Varint packetID(0x42);
        packet << packetID;
        Coordinate<int> spawnPos(0,6,0);
        packet << spawnPos;

        packet.addMsgLen();
        owner->sendMessage(packet);
    };
};

#endif
