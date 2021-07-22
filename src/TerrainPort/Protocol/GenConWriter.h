#pragma once

#include "../../Protocol/PacketWriter.h"



class GenConWriter : public PacketWriter{
public:
    void writeHandshake(string address = "localhost", unsigned short port = 25565);
    void writeLoginStart(string name);
    void writePosAndLook(Coordinate<double> pos, float yaw, float pitch, bool onGround = true);
};
