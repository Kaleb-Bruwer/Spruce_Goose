#ifndef CLICKWINDOWJOB_H
#define CLICKWINDOWJOB_H

#include "../JobTicket.h"

using namespace std;

class ClickWindowJob : public JobTicket{
public:
    ClickWindowJob() : JobTicket(CLICKWINDOWJOB){};

    int eid;

    char windowID = 0;
    short slotNum = 0;
    char button = 0;
    short actionNum = 0; //Used in ConfirmTransaction
    bool shift = 0;
    char mode = 0;
};

#endif
