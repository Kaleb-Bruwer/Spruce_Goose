#include "GenConWriter.h"

void GenConWriter::writeHandshake(string address, unsigned short port){
    writePacketID(0x0);
    baseThis << Varint(4); //Protocol version
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
