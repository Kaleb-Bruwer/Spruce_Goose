#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

#include "ConnectionHandler.h"

using namespace std;

class World;

class Protocol{
private:
    World* world = 0;

    int portNum;
    int sockfd; //Will only contain players trying to open connections
    int maxPlayersPerHandler = 10000;
    bool quit = false;
    bool multiT;

    int nextAddPos = 0;//Keeps track of which ConnectionHandler has an open slot
    struct sockaddr_in servAddr;
    vector<ConnectionHandler*> connectionHandlers;
    thread* protocolMainThread = 0;

    void openSocket();
    void acceptConnections();
    void addPlayer(int socket);

public:
    Protocol(int portN, World* w, bool multiT = true);
    ~Protocol();
    void join();

    void loop();
};

#endif
