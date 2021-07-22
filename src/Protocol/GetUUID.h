#ifndef GETUUID_H
#define GETUUID_H

#include <cstring>
#include <curl/curl.h>
#include <thread>
#include <string>

using namespace std;

class PlayerConnection;

size_t callbackGetUUID(void* contents, size_t size, size_t nmemb, void *userp);

class GetUUID{
private:
    PlayerConnection* player;
    string username;
    thread* myThread;
    void sendIt();
public:
    GetUUID(PlayerConnection* player, string username);
    ~GetUUID();
};

#endif
