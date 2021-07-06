#ifndef CLIENTSLOTCHANGE_H
#define CLIENTSLOTCHANGE_H

#include "../JobTicket.h"
#include "../../Inventory/Slot.h"

using namespace std;

class ClientSlotChange : public JobTicket{
public:
    ClientSlotChange() : JobTicket(CLIENTSLOTCHANGE){};

    int eid; //To find player with
    short slotNum = -1; //if -1, no item change
    Slot* slot = 0;
    short cursor = -1; //if -1, it doesn't change
};

#endif
