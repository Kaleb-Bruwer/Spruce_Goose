#ifndef CHATTOWORLD_H
#define CHATTOWORLD_H

#include "../JobTicket.h"

#include <string>

using namespace std;

class ChatToWorld : public JobTicket{
public:
    string username;
    string message;

    ChatToWorld(string s) : JobTicket(CHATTOWORLD){
        message = s;
    };
};

#endif
