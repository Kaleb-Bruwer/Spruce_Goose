#ifndef PLAYERCONNECTION_H
#define PLAYERCONNECTION_H

#include <string>
#include <chrono>

#include "PacketWriter.h"
#include "PacketReader.h"
#include "GetUUID.h"
#include "./PacketHandlers/PacFullChunk.h"
#include "../Datastructures/Varint.h"
#include "../Datastructures/BlockingQueue.h"
#include "../JobTickets/JobTicket.h"

using namespace std;

class PacketHandlerRead;
class PacketHandlerWrite;
class PacFullChunk;
class World;
class ThreadArea;

class PlayerConnection{
protected:
    int state = 0;
    World* world = 0;
    bool loggedIn = false;

    mutex followingMutex;
    vector<ThreadArea*> followingAreas; //Used to exit safely
    GetUUID* getUUIDObject = 0;

    int protocolVer = 0;

    chrono::high_resolution_clock::time_point nextKeepAlive;
    vector<int> pendingKeepAlives;

    BlockingQueue<JobTicket*> toPlayerQueue;

    void getUUID();//Call AFTER username has been set
    void genRandomUUID();

    virtual void sendChat(JobTicket* job) = 0;
    virtual void sendChunks(JobTicket* job){};
    virtual void receiveUUID(JobTicket* job);
    virtual void sendSpawnPos(JobTicket* job) = 0;
    virtual void sendPosAndLook(JobTicket* job) = 0;
    virtual void sendItem(JobTicket* job){};
    virtual void sendPlayer(JobTicket* job){};
    virtual void sendWindowItem(JobTicket* job){};
    virtual void blocksToPlayer(JobTicket* job){};
    virtual void entityMove(JobTicket* job){};
    virtual void destroyEntity(JobTicket* job){};
    virtual void sendWindowOpenClose(JobTicket* job){};
    virtual void sendConfirmTransaction(JobTicket* job){};

    virtual void receivePlayerLogin(JobTicket* job);
    virtual void spawnIn(JobTicket* job) = 0;

    void joinWorld();

    friend class PacketHandlerRead;
    friend class PacketHandlerWrite;
    friend class PacFullChunk;

public:
    PlayerConnection(int sock, World* w);
    virtual ~PlayerConnection();

    void sendMessage(const char* start, int length);
    void sendMessage(PacketWriter &p);
    void sendMessage(PacketWriter* p); //To support children of PacketWriter
    void sendMessage(JobTicket* job); //job of type SendPacket

    int eid = 0;//Used to find PlayerEntity
    string username;
    int gamemode = 0;
    char uuid[16];
    string uuidString;
    int mySocket;
    bool quit = false; //No packets can be sent if this is true

    virtual void loginRoutine();

    void setState(int s){state = s;};
    void pushJobToPlayer(JobTicket* job); //To client

    //May add params to handle data transfer to threadArea objects
    //Message is everything from the packetID
    virtual void handleMessage(PacketReader &p) = 0;
    virtual void handleMessage(char* start, int size) = 0;

    //Since it depends on player version
    //2nd parameter is either the index or length. Use 0 for unknown length
    //PacketReader will increase index by packet length
    //PacketReaderOld will increase index as it reads
    virtual void handleJobTickets();

    virtual bool sendKeepAlive();
    void receiveKeepAlive(int id);

    void disconnect();
    bool canDelete();
    int getNThreadAreas();

    void addThreadArea(ThreadArea*);
    void removeThreadArea(ThreadArea*);

    BlockingQueue<JobTicket*> toServerQueue; //JobTickets coming from Protocol
    void pushJobToServer(JobTicket* job); //to ThreadArea

};

#endif
