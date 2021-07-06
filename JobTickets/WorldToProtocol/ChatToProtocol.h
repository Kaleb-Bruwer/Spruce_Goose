#ifndef CHATTOPROTOCOL_H
#define CHATTOPROTOCOL_H

#include "../JobTicket.h"

#include <string>

using namespace std;

class ChatToProtocol : public JobTicket{
public:
    string message;

    ChatToProtocol(string s){
        message = s;
    }
    ChatToProtocol(ChatToProtocol* c){
        message = c->message;
    }

    string getType(){
        return "ChatToProtocol";
    }
};

#endif
