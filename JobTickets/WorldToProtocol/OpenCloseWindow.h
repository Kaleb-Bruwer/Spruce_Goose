#ifndef OPENCLOSEWINDOW_H
#define OPENCLOSEWINDOW_H

#include "../JobTicket.h"

using namespace std;

class OpenCloseWindow : public JobTicket{
public:
    bool open;
    unsigned char windowID;
    unsigned short numSlots;
    string name;

    string getType(){return "OpenCloseWindow";};
};

#endif
