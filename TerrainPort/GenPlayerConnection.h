#pragma once

#include <string>

class GenPlayer;

class GenPlayerConnection{
private:
    int sock = 0;
    struct sockaddr_in serv_addr;


    friend class GenPlayer;

    void openConnection(string addr, int port);
    void closeConnection();

};
