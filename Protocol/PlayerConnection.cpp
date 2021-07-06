#include "PlayerConnection.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <unistd.h> //write()

#include "ProtocolHelpers.h"
#include "../General/LevelData.h"
#include "../World/World.h"
#include "../JobTickets/JoinWorld.h"
#include "../JobTickets/GivePlayerUUID.h"
#include "../JobTickets/SendPacket.h"
#include "../JobTickets/WorldToProtocol/SendThreadArea.h"
#include "../JobTickets/WorldToProtocol/SendPlayerLogin.h"

using namespace std;

//PROTOCOL STATES:
//0: Handshaking
//1: Status
//2: Login
//3(?): Play

PlayerConnection::PlayerConnection(int sock, World* w){
    nextKeepAlive = chrono::high_resolution_clock::now();
    mySocket = sock;
    world = w;

    LevelData* l = LevelData::getInstance();
    gamemode = l->getGamemode();
}

PlayerConnection::~PlayerConnection(){
    if(getUUIDObject)
        delete getUUIDObject;

    if(mySocket >= 0){
        cout << "Closing socket " << mySocket << endl;
        close(mySocket);
    }
}

void PlayerConnection::getUUID(){
    getUUIDObject = new GetUUID(this, username);
}

void PlayerConnection::genRandomUUID(){
    for(int i=0; i<16; i++)
        uuid[i] = rand()%256;
}

void PlayerConnection::sendMessage(const char* start, int length){
    if(!quit){
        // if(length < 128){
        //     cout << "To " << username << "\n\t";
        //     for(int i=0; i< length; i++){
        //         cout << (int) (unsigned char) start[i] << " ";
        //     }
        //     cout << endl;
        // }
        // else{
        //     cout << "Sending large packet with [0] = " <<(int) (unsigned char)start[0] << "\n\n";
        // }

        write(mySocket, start, length);
    }
}

void PlayerConnection::sendMessage(PacketWriter &p){

    /*
    if(p.getIndex() < 128){
        cout << "To " << username << " ";
        p.print();
        cout << "\n";
    }
    else{
        cout << "Sending large packet with [0] = " ;
        cout << (int)(unsigned char)p.getBuffer()[0] << "\n\n";
    }
    */

    sendMessage(p.getBuffer(), p.getIndex());
}

void PlayerConnection::sendMessage(PacketWriter *p){
    sendMessage(p->getBuffer(), p->getIndex());
}

void PlayerConnection::sendMessage(JobTicket* j){
    SendPacket* job = (SendPacket*) j;
    sendMessage(job->data, job->size);
}

void PlayerConnection::pushJobToPlayer(JobTicket* job){
    job->pickup();
    toPlayerQueue.push(job);
}

void PlayerConnection::pushJobToServer(JobTicket* job){
    job->pickup();
    toServerQueue.push(job);
}

void PlayerConnection::handleJobTickets(){
    if(quit)
        return;

    while(true){
        JobTicket* job = toPlayerQueue.pop();
        if(job == 0)
            break;

        string jobType = job->getType();

        //Notice that none of the functions called below should delete the jobTicket,
        //It is deleted right here.
        if(jobType == "SendChunks"){
            sendChunks(job);
            return; //Must wait for chunks to complete to preserver order
        }
        else if(jobType == "SendPlayerPos"){
            sendPosAndLook(job);
        }
        else if(jobType == "GivePlayerUUID"){
            receiveUUID(job);
        }
        else if(jobType == "SendPlayerLogin"){
            receivePlayerLogin(job);
        }
        else if(jobType == "SendThreadArea"){}
        else if(jobType == "ChatToProtocol"){
            sendChat(job);
        }
        else if(jobType == "SendItem"){
            sendItem(job);
        }
        else if(jobType == "SendWindowItem"){
            sendWindowItem(job);
        }
        else if(jobType == "BlockUpdateToPlayer"){
            blocksToPlayer(job);
        }
        else if(jobType == "SendPlayer"){
            sendPlayer(job);
        }
        else if(jobType == "EntityMove"){
            entityMove(job);
        }
        else if(jobType == "DestroyEntityJob"){
            destroyEntity(job);
        }
        else if(jobType == "SendPacket"){
            sendMessage(job);
        }
        else if(jobType == "OpenCloseWindow"){
            sendWindowOpenClose(job);
        }
        else if(jobType == "CanSpawn"){
            spawnIn(job);
        }
        else if(jobType == "ConfirmTransaction"){
            sendConfirmTransaction(job);
        }
        job->drop();
    }

    sendKeepAlive(); //Only sends it if it's time
}

void PlayerConnection::loginRoutine(){
    /*
    //Send LoginSuccess
    sendLoginSuccess();
    sendSpawnPos((JobTicket*)0); //Move this when spawn can be changed

    //Join a ThreadArea, it will send game data through JobTickets later
    joinWorld();

    //The old routine:
    //wLoginSuccess()
    //wSpawnPosition();
    //wInventory();
    //wPositionAndLook();
    //sendArea(); //Sent a bunch of chunks
    */
}

void PlayerConnection::receiveUUID(JobTicket* j){
    GivePlayerUUID* job = (GivePlayerUUID*)j;
    delete getUUIDObject;
    getUUIDObject = 0;

    uuidString = job->uuid;
}

void PlayerConnection::receivePlayerLogin(JobTicket* j){
    eid = ((SendPlayerLogin*)j)->eid;
}

void PlayerConnection::joinWorld(){
    //Send jobTicket to World
    //World loads player's file
    //According to player position, World assigns player to a ThreadArea
    JobTicket* job = new JoinWorld(this);
    world->pushJob(job);
}

bool PlayerConnection::sendKeepAlive(){
    //child classes must still call this version from theirs,
    //but this isn't a complete implementation. It simply handles the timing
    if(chrono::high_resolution_clock::now() > nextKeepAlive){
        nextKeepAlive += chrono::seconds{1};
        return true;
    }
    else
        return false;
}

void PlayerConnection::receiveKeepAlive(int id){
    //if id matches any of those being waited for, the list will be cleared
    //otherwise it will be ignored
    //This data can later be used to kick players who aren't responding
    auto it = find(pendingKeepAlives.begin(), pendingKeepAlives.end(), id);
    if(it != pendingKeepAlives.end()){
        pendingKeepAlives.clear();
    }
}

void PlayerConnection::disconnect(){
    quit = true;
}

bool PlayerConnection::canDelete(){
    return (quit && (getNThreadAreas() == 0));
}

int PlayerConnection::getNThreadAreas(){
    followingMutex.lock();

    int result = followingAreas.size();
    followingMutex.unlock();

    return result;
}

void PlayerConnection::addThreadArea(ThreadArea* t){
    followingMutex.lock();

    followingAreas.push_back(t);

    followingMutex.unlock();
}

void PlayerConnection::removeThreadArea(ThreadArea* t){
    followingMutex.lock();

    auto it = find(followingAreas.begin(), followingAreas.end(), t);
    followingAreas.erase(it);

    followingMutex.unlock();
}
