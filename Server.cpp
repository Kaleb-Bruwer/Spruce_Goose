#include "Server.h"

#include <iostream>

#include "./Datastructures/Coordinate/ChunkCoord.h"

Server::Server(int portnum, bool multithreaded){
    cout << "Creating server...\n";
    if(multithreaded)
        multiT(portnum);
    else{
        singleT(portnum);
        singleTLoop();
    }
}

Server::~Server(){
    cout << "Server destructor\n";
    p->join();
    delete p;
    delete world;
    cout << "Goodbye\n";
}

void Server::multiT(int portnum){
    world = new World();
    p = new Protocol(portnum, world);
    commandLine();
}

void Server::singleT(int portnum){
    world = new World(false);
    p = new Protocol(portnum, world, false);
}

void Server::singleTLoop(){
    while(true){
        p->loop();
        world->loop();
    }
}

void Server::commandLine(){
    cout << "Ready for commands\n";
    while(true){
        string line;
        cin >> line;

        if(line[0] ==  'q'){ //Quit
            break;
        }
    }
    cout << "Command line closed\n";
}
