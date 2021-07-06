#pragma once

#include "../JobTicket.h"

using namespace std;

class ConfirmTransaction : public JobTicket{
public:
    int windowID = 0;
    int actionNum = 0;
    bool accepted = true;

    string getType(){return "ConfirmTransaction";};
};
