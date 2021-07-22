#pragma once

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class GenPlayer;

class GenPlayerConnection{
private:
    bool isOpen = false;
    int sock = -1;
    struct sockaddr_in serv_addr;


public:

    void openConnection(std::string addr, int port);
    void closeConnection();

    bool getIsOpen(){return isOpen;};

};
