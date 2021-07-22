#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include "./Protocol/Protocol.h"
#include "./World/World.h"

using namespace std;

class Server{
private:
    Protocol* p = 0;
    World* world = 0; //Encapsulates all of it, not just 1 dimension

    void multiT(int portnum);
    void singleT(int portnum);
    void singleTLoop();

    void commandLine();
public:
    Server(int portnum = 25565, bool multithreaded = true);
    ~Server();
};

#endif
