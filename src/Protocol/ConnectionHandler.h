#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <thread>
#include <mutex>
#include <list>
#include <map>
#include <set>

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../Datastructures/Varint.h"
#include "PlayerConnection.h"

using namespace std;

class World;

class ConnectionHandler{
private:
    int epfd;
    static const int maxPerRun = 50;
    struct epoll_event events[maxPerRun];

    map<int, PlayerConnection*> players;
    set<int> playersNoVersion;
    int maxCap;
    int currSize = 0;
    int newSize = 0;
    thread* myThread = 0;
    bool quit = false;

    World* world;

    mutex incomingLock;
    mutex removeLock;
    list<PlayerConnection*> incomingPlayers;
    list<int> incomingSockets;//Players who still need to shake hands (covid's gonna make this hard)
    list<string> removePlayers;

    void run();

    void readSocket(int sock);
    void readNoVersion(int sock);
    void readForPlayer(int sock);

    void includeNewPlayers();
    void removeConnections();
    //No remSock needed, closing socket automatically removes it
    void addSockToEP(int sock);

    void updatePlayers();
    void handleJobTickets();

    void sendOldMOTD(int sock);

public:
    ConnectionHandler(int cap, World* w, bool multiT = true);
    ~ConnectionHandler();
    void join();
    bool addPlayer(PlayerConnection* p);//false if it couldn't be added
    bool addPlayer(int sock);

    void loopBody(); //Used for singleTreaded mode
};

#endif
