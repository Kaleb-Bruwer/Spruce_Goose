#include "ConnectionHandler.h"

#include <cstring>
#include <iostream>
#include <chrono>

#include "../World/World.h"

#include "ProtocolHelpers.h"
#include "PacketReader.h"

#include "PacketWriterOldMOTD.h"

#include "PlayerConnection1_7.h"

#include "../JobTickets/ProtocolToWorld/ChatToWorld.h"

ConnectionHandler::ConnectionHandler(int cap, World* w, bool multiT){
    world = w;

    maxCap = cap;//maxCapacity
    epfd = epoll_create(maxCap);

    if(multiT)
        myThread = new thread(&ConnectionHandler::run, this);
}

ConnectionHandler::~ConnectionHandler(){
    if(myThread){
        join();
    }

    for(auto it = players.begin(); it != players.end(); it++){
        if(it->second)
            delete it->second;
    }

    for(auto it = incomingPlayers.begin(); it != incomingPlayers.end(); it++){
        delete (*it);
    }
}

void ConnectionHandler::join(){
    quit = true;
    myThread->join();
    delete myThread;
    myThread = 0;
}

void ConnectionHandler::handleJobTickets(){
    for(auto it = players.begin(); it != players.end(); it++){
        it->second->handleJobTickets();
    }
}

bool ConnectionHandler::addPlayer(int sock){
    incomingLock.lock();
    if(newSize >= maxCap){
        incomingLock.unlock();
        return false;
    }
    newSize++;
    incomingSockets.push_back(sock);

    incomingLock.unlock();
    return true;
}

bool ConnectionHandler::addPlayer(PlayerConnection* p){
    incomingLock.lock();
    if(newSize >= maxCap){
        incomingLock.unlock();
        return false;
    }
    newSize++;
    incomingPlayers.push_back(p);

    incomingLock.unlock();
    return true;
}

void ConnectionHandler::addSockToEP(int sock){
    cout << "new connection " << sock << endl;
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    ev.data.fd = sock;
    int res = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);

    if(res < 0){
        cout << "epoll_ctl ERROR in ConnectionHandler::addSockToEP\n";
        cout << "Response " << res << " when adding sock = " << sock << endl;
    }
}

void ConnectionHandler::includeNewPlayers(){
    incomingLock.lock();
    for(auto it = incomingPlayers.begin(); it != incomingPlayers.end(); it = incomingPlayers.erase(it)){
        int sock = (*it)->mySocket;
        players[sock] = (*it);

        addSockToEP(sock);
        currSize++;
    }

    for(auto it = incomingSockets.begin(); it != incomingSockets.end(); it++){
        playersNoVersion.insert(*it);
        addSockToEP(*it);
        currSize++;
    }
    incomingSockets.clear();

    newSize = currSize;
    incomingLock.unlock();
}

void ConnectionHandler::removeConnections(){
    //Delete PlayerConnections where appropriate
    //socket is closed by PlayerConnection destructor
    for(auto it = players.begin(); it != players.end();){
        PlayerConnection* p = it->second;
        if(p->canDelete()){
            ChatToWorld* job = new ChatToWorld("");
            job->message = p->username + " has left the game";
            world->pushJob(job);
            delete p;
            it = players.erase(it);
        }
        else{
            it++;
        }
    }
}

void ConnectionHandler::loopBody(){
    includeNewPlayers();
    handleJobTickets();
    removeConnections();

    //timeout is in ms and is 5x shorter than a game tick so
    //that writes don't get delayed too much, since they're on the same thread
    const int timeout = 10;
    int numToRead = epoll_wait(epfd, &(events[0]), maxPerRun, timeout);

    for(int i=0; i<numToRead; i++){
        readSocket(events[i].data.fd);
    }

}

void ConnectionHandler::readSocket(int sock){
    //First figure out if it's a socket with a PlayerConnection or not

    if(playersNoVersion.find(sock) != playersNoVersion.end()){
        //in this case it's a versionless socket
        readNoVersion(sock);
    }
    else{
        //in this case there's a PlayerConnection for the socket
        readForPlayer(sock);
    }
}

void ConnectionHandler::readForPlayer(int sock){
    const int bufferSize = 8192;
    char buffer[bufferSize];

    int lenRead = read(sock, buffer, bufferSize);

    //Get correct player
    //Using [] operator because player is supposed to be there
    PlayerConnection* p = players[sock];

    if(lenRead <=0){
        // Close connection (this is a long process, has to be removed from world first)
        p->quit = true;
        return;
    }

    if(!p){
        //Doubt this will ever happen
        cout << "Weird case in ConnectionHandler::readForPlayer()\n";
        return;
    }

    /*
    if(p->username == "BlueC0dex"){
        cout << "Reading message from BlueC0dex\n";
    }
    */

    p->handleMessage(buffer, lenRead);

}

void ConnectionHandler::readNoVersion(int sock){
    const int bufferSize = 8192;
    char buffer[bufferSize];

    int lenRead = read(sock, buffer, bufferSize);

    if(lenRead <= 0){
        // must close socket
        playersNoVersion.erase(sock);
        currSize--;
        close(sock);
        return;
    }
    // After this it is known that at least 1 byte was read

    int state = 0;
    int protocolVersion;
    string username = "";

    int index = 0;

    /*
    //Uncomment to see raw packets
    cout << "[" << lenRead << "] ";
    for(int i=-4; i<0; i++){
        if(index + i >=0){
            cout << hex << (int)((unsigned char)buffer[index + i]) << ' ';
        }
    }
    cout << "| ";
    for(int i=0; i<16;i++){
        cout << hex << (int)((unsigned char)buffer[index + i]) << ' ';
    }
    cout << endl;
    */

    if(buffer[index] == 2){
        //Old handshake, new one will never start with 2
        playersNoVersion.erase(sock);
        currSize--;
        close(sock);
        return;
    }
    else if (((unsigned char)buffer[index] == 0xFE)){
        //Remove connection
        playersNoVersion.erase(sock);
        currSize--;
        sendOldMOTD(sock);
        close(sock);
        return;
    }
    else{
        //Modern handshake
        PacketReader reader(&(buffer[0]), lenRead);

        PlayerConnection* newPlayer = new PlayerConnection1_7(sock, world);
        newPlayer->handleMessage(reader);

        playersNoVersion.erase(sock);
        players[sock] = newPlayer;

        /*
        PacketReader reader(&(buffer[index]), index);

        int packetID = reader.readVarint().getInt();
        protocolVersion = reader.readVarint().getInt();
        string addr = reader.readString();
        short portNum = reader.readShort();
        state = reader.readVarint().getInt();

        if(state != 1)//Only 2 accepted states
            state == 2;

        */
    }

    /*
    string output = "New connection, version " + to_string(protocolVersion) +
        ", username " + username + '\n';
    cout << output;
    */
}

void ConnectionHandler::sendOldMOTD(int sock){
    cout << "ConnectionHandler::sendOldMOTD\n";
    //Send response to player
    string prVersion = "754"; //1.16.(4/5)
    string mcVersion = "1.16.5";
    string motd = "Your client is older than electricity, please update";
    string playerCount = "0";
    string playerCap = "0";

    PacketWriterOldMOTD writer;
    writer.writePacketID(0xFF);
    writer.nullSeparator(); //Held open for length field
    writer.weirdSeparator();
    writer.nullSeparator();
    writer << prVersion;
    writer.nullSeparator();
    writer << mcVersion;
    writer.nullSeparator();
    writer << motd;
    writer.nullSeparator();
    writer << playerCount;
    writer.nullSeparator();
    writer << playerCap;
    writer.writeLenInGap();
    writer.sendPacket(sock);
}

void ConnectionHandler::run(){
    while(true){
        loopBody();
    }
}
