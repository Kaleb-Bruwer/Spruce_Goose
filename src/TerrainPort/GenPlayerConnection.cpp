#include "GenPlayerConnection.h"

#include <iostream>

using namespace std;

void GenPlayerConnection::openConnection(string address, int port){
    if(sock >= 0){
        close(sock);
        sock = -1;
    }

    isOpen = false;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Couldn't create socket in GenPlayerConnection::openConnection\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)<=0){
        cout << "Address " << address << " is not valid\n";
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        cout << "Connection failed in GenPlayerConnection::openConnection\n";
        return;
    }

    isOpen = true;
}


void GenPlayerConnection::closeConnection(){
    if(sock >= 0)
        close(sock);
    sock = -1;
}
