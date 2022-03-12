#ifndef CLOSEWINDOWJOB_H
#define CLOSEWINDOWJOB_H

#include "../JobTicket.h"

using namespace std;

class CloseWindowJob : public JobTicket{
public:
    CloseWindowJob() : JobTicket(CLOSEWINDOWJOB){};

    unsigned char windowID;
};

#endif
