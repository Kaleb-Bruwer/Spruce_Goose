#include "GenConWriter.h"

void GenConWriter::writeHandshake(string address, unsigned short port){
    writePacketID(0x0);
    baseThis << Varint(5); //Protocol version
    baseThis << address;
    baseThis << port;
    baseThis << Varint(2); //State

    addMsgLen();
}

void GenConWriter::writeLoginStart(string name){
    writePacketID(0x0);
    baseThis << name;
    addMsgLen();
}

void GenConWriter::writePosAndLook(Coordinate<double> pos, float yaw, float pitch, bool onGround){
    writePacketID(0x06);

    baseThis << pos.x;
    baseThis << pos.y;
    baseThis << pos.y + 1.62; //head pos
    baseThis << pos.z;

    baseThis << yaw;
    baseThis << pitch;
    baseThis << onGround;

    addMsgLen();
}
