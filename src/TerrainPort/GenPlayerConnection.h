#pragma once

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>

#include "../Protocol/PacketWriter.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/Coordinate/ChunkCoord.h"


class GenPlayer;
class Chunk;

class GenPlayerConnection{
private:
    bool pendingLookUpdate = false;
    chrono::high_resolution_clock::time_point lookUpdateTime;

    bool isOpen = false;
    int sock = -1;
    struct sockaddr_in serv_addr;

    int connState = 0; // Connection state, as in the Minecraft protocol
    bool hasSpawned = false;
    Coordinate<double> pos;
    float yaw;
    float pitch;
    bool onGround;

    bool mustTP;
    Coordinate<int> pendingTP;

    friend class GenPlayer;

    void checkDelayedPlayerLook();

public:
    void sendMessage(PacketWriter &p);
    vector<Chunk*> readMessage();

    void returnSpawnInPos();

    void openConnection(std::string addr, int port, std::string username);
    void closeConnection();

    void handshake(string username);
    void sendTeleport(Coordinate<int> coord);

    bool getIsOpen(){return isOpen;};
    bool getHasSpawned(){return hasSpawned;};

};
