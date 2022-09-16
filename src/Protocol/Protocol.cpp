# include "Protocol.h"

#include "ProtocolHelpers.h"
#include "../Datastructures/Varint.h"

Protocol::Protocol(int portN, World* w, bool multiT){
    world = w;

    this->multiT = multiT;
    portNum = portN;
    connectionHandlers.push_back(new ConnectionHandler(maxPlayersPerHandler, w, multiT));

    openSocket();
    if(multiT)
        protocolMainThread = new thread(&Protocol::acceptConnections, this);
}

void Protocol::join(){
    if(!protocolMainThread)
        return;
    quit = true;
    protocolMainThread->join();
    delete protocolMainThread;
    protocolMainThread = 0;
    for(int i=0; i < connectionHandlers.size(); i++){
        connectionHandlers[i]->join();
        delete connectionHandlers[i];
    }
}

Protocol::~Protocol(){
    join();
}

void Protocol::openSocket(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        cout <<"Could not open socket\n";
        return;
    }
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(portNum);
    if(bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0){
        cout << "Could not bind to socket\n";
        return;
    }
    listen(sockfd, 16);

    cout << "Master socket is open\n";
}

void Protocol::loop(){
    /*
    if(!multiT){
        for(int i=0; i<connectionHandlers.size(); i++){
            connectionHandlers[i]->loop();
        }
    }
    */

    struct sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    int clientSocket = accept(sockfd, (struct sockaddr*)&clientAddr, (socklen_t*)&clientLen);
    if(clientSocket < 0){
        if(errno == EMFILE){
            cout << "Process out of fds\n";
        }
        else if(errno == ENFILE){
            cout << "Operating Sysyem out of fds\n";
        }
        else{
            cout << "Unknown error (" << errno << ") in Protocol\n";
        }
        throw errno;
    }
    else{
        addPlayer(clientSocket);
    }
}

void Protocol::addPlayer(int sock){
    //hand off to a ConnectionHandler
    nextAddPos--;
    do{
        nextAddPos++;
        if(nextAddPos >= connectionHandlers.size()){
            connectionHandlers.push_back(new ConnectionHandler(maxPlayersPerHandler, world, multiT));
        }
    }while(!connectionHandlers[nextAddPos]->addPlayer(sock));
}

void Protocol::acceptConnections(){
  //TODO: Add a way to break this loop
    while(true){
        if(quit)
            return;
        loop();
    }
}
