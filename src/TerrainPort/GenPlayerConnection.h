#pragma once

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../Protocol/PacketWriter.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/Coordinate/ChunkCoord.h"


class GenPlayer;
class Chunk;

class GenPlayerConnection{
private:
    bool isOpen = false;
    int sock = -1;
    struct sockaddr_in serv_addr;

    int connState = 0; // Connection state, as in the Minecraft protocol
    bool hasSpawned = false;
    Coordinate<double> pos;
    float yaw;
    float pitch;
    bool onGround;

    friend class GenPlayer;

public:
    void sendMessage(PacketWriter &p);
    vector<Chunk*> readMessage();

    void returnSpawnInPos();

    void openConnection(std::string addr, int port);
    void closeConnection();

    void handshake();
    void sendTeleport(ChunkCoord coord);

    bool getIsOpen(){return isOpen;};
    bool getHasSpawned(){return hasSpawned;};

};
