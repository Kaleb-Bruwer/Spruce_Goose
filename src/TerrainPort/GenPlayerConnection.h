#pragma once

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../Protocol/PacketWriter.h"
#include "../Datastructures/Coordinate/Coordinate.h"


class GenPlayer;

class GenPlayerConnection{
private:
    bool isOpen = false;
    int sock = -1;
    struct sockaddr_in serv_addr;

    int connState = 0; // Connection state, as in the Minecraft protocol
    Coordinate<double> pos;
    float yaw;
    float pitch;

    void sendMessage(PacketWriter &p);
    void readMessage();

public:

    void openConnection(std::string addr, int port);
    void closeConnection();

    void handshake();

    bool getIsOpen(){return isOpen;};

};
