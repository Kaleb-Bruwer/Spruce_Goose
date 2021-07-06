#pragma once
#include "PlayerConnection.h"

class PlayerConnection1_7 : public PlayerConnection{
private:
    bool keepAliveSafe = false; //Sending a keepAlive too soon crashes client

protected:

    void handleJobTickets();

    void sendChat(JobTicket* job);
    void sendSpawnPos(JobTicket* job);
    void sendPosAndLook(JobTicket* job);
    bool sendKeepAlive();
    void sendItem(JobTicket* job);
    void sendPlayer(JobTicket* job);
    void entityMove(JobTicket* job);
    void sendWindowItem(JobTicket* job);
    void destroyEntity(JobTicket* job);
    void sendConfirmTransaction(JobTicket* job);

    void sendChunks(JobTicket* job){};
    void blocksToPlayer(JobTicket* job){};
    void sendWindowOpenClose(JobTicket* job){};

    void receivePlayerLogin(JobTicket* job);
    void spawnIn(JobTicket* job);

    //state = 0
    void readHandshake(PacketReader &p);

    //state = 1
    void readSLP(PacketReader &p);
    void readPing(PacketReader &p, bool mustClose = false);

    //state = 2
    void readLoginStart(PacketReader &p);

    //state = 3
    void readKeepAlive(PacketReader &p);
    void readChat(PacketReader &p);
    //Player (0x03) deliberately ignored
    void readPlayerPosition(PacketReader &p);
    void readPlayerLook(PacketReader &p);
    void readPlayerPosAndLook(PacketReader &p);
    void readPlayerDigging(PacketReader &p);
    void readPlayerBlockPlacement(PacketReader &p);

    void readClickWindow(PacketReader &p);

public:
    PlayerConnection1_7(int sock, World* w);
    ~PlayerConnection1_7();
    void handleMessage(PacketReader &p);
    void handleMessage(char* start, int size);

};
