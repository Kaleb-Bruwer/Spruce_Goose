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

        bool bufferEmpty = false;

        if(end != 0){
            int ret = write(mySocket, buffer, end);

            if(ret == end){
                bufferEmpty = true;
                end = 0;
                size = 0;
                delete [] buffer;
                buffer = 0;
            }
            if(ret == -1)
                quit = true;
            else if(ret < end && ret > 0){
                // Not all was sent
                int remain = end - ret;
                memmove(buffer, &(buffer[ret]), remain);
                end -= ret;
            }
        }

        int ret;
        if(bufferEmpty)
            ret = write(mySocket, start, length);
        else
            ret = 0; //Will add to buffer in this case

        if(ret == -1){
            // An error has occured
            quit = true;
        }
        else if(ret < length){
            // Not all data was sent, move to buffer
            // 0 means no data was sent, but not due to an error
            int remain = length - ret;
            if(size - end >= remain){ //enough space in buffer
                memcpy(&(buffer[end]), &(start[ret]), remain);
                end += remain;
            }
            else if(buffer == 0){
                // There is no buffer
                size = max(1024, remain * 2);
                buffer = new char[size];
                memcpy(buffer, &(start[ret]), remain);
                end = remain;
            }
            else{
                // there is a buffer, but it's too small
                size = max(1024, end + remain * 2);
                char* oldBuffer = buffer;
                buffer = new char[size];
                memcpy(buffer, oldBuffer, end);
                memcpy(&(buffer[end]), &(start[ret]), remain);
                end += remain;

                delete [] oldBuffer;
            }
        }
        //else everything was fine

    }
}

void PlayerConnection::sendMessage(PacketWriter &p){
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
