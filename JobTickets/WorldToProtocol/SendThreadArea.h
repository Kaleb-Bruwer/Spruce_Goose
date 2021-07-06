#ifndef SENDTHREADAREA_H
#define SENDTHREADAREA_H

#include "../JobTicket.h"

using namespace std;

class ThreadArea;

class SendThreadArea : public JobTicket{
public:
    ThreadArea* threadArea = 0;

    string getType(){
        return "SendThreadArea";
    }
};

#endif
