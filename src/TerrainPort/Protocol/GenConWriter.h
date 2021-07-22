#pragma once

#include "../../Protocol/AdvancedWriter.h"


// Private cuts off use of AdvancedWriter's packets, while keeping its
// verison of addMsgLen
class GenConWriter : private AdvancedWriter{
public:
    void writeHandshake(string address = "localhost", unsigned short port = 25565);
    void writeLoginStart(string name);
    void writePosAndLook(Coordinate<double> pos, float yaw, float pitch, bool onGround = true);
};
