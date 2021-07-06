#ifndef CLICKWINDOWJOB_H
#define CLICKWINDOWJOB_H

#include "../JobTicket.h"

using namespace std;

class ClickWindowJob : public JobTicket{
public:
    int eid;

    char windowID;
    short slotNum;
    char button;
    short actionNum; //Used in ConfirmTransaction
    bool shift;
    char mode;

    string getType(){return "ClickWindowJob";};
};

#endif
