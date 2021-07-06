#ifndef CLOSEWINDOWJOB_H
#define CLOSEWINDOWJOB_H

#include "../JobTicket.h"

using namespace std;

class CloseWindowJob : public JobTicket{
public:
    unsigned char windowID;

    string getType(){return "CloseWindowJob";};
};

#endif
