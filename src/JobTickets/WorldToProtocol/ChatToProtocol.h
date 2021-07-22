#ifndef CHATTOPROTOCOL_H
#define CHATTOPROTOCOL_H

#include "../JobTicket.h"

#include <string>

using namespace std;

class ChatToProtocol : public JobTicket{
public:
    string message;

    ChatToProtocol(string s) : JobTicket(CHATTOPROTOCOL){
        message = s;
    }
    ChatToProtocol(ChatToProtocol* c) : JobTicket(CHATTOPROTOCOL){
        message = c->message;
    }
};

#endif
