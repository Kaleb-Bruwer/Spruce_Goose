#include "GetUUID.h"

#include "PlayerConnection.h"
#include "../JobTickets/GivePlayerUUID.h"

using namespace std;

GetUUID::GetUUID(PlayerConnection* p, string u){
    player = p;
    username = u;
    myThread = new thread(&GetUUID::sendIt, this);
}

GetUUID::~GetUUID(){
    myThread->join();
    delete myThread;
}

void GetUUID::sendIt(){
    CURL* curl_handle = curl_easy_init();

    string url = "api.mojang.com/users/profiles/minecraft/" + username;

    if(curl_handle){
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, callbackGetUUID);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)player);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        curl_easy_perform(curl_handle);

        curl_easy_cleanup(curl_handle);
    }
}

size_t callbackGetUUID(void* contents, size_t size, size_t nmemb, void *myParam){
    PlayerConnection* player = (PlayerConnection*) myParam;

    string result((char*)contents, nmemb);

    //Get UUID from the json
    int pos = result.find(",\"id\":");
    if(pos >= result.length())
        return size* nmemb;
    pos += 5;
    string uuid = result.substr(pos,32);
    //return uuid as string (not the compact 16-bit version)
    GivePlayerUUID* job = new GivePlayerUUID(uuid);
    player->pushJobToPlayer(job);

    return size * nmemb;
}
