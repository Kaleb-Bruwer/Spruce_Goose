#ifndef SENDWINDOWITEM_H
#define SENDWINDOWITEM_H

#include "../JobTicket.h"

#include <algorithm>
#include <vector>

#include "../../Inventory/Slot.h"

using namespace std;

class SendWindowItem : public JobTicket{
public:
    int windowID = 0; //Needed in 1.7

    int numSlots = 0;

    vector<int> slotNums;
    vector<Slot> slots;

    SendWindowItem() : JobTicket(SENDWINDOWITEM){};

    ~SendWindowItem(){};

    void addSlot(int s, Slot &data){ //makes deep copy
        auto it = find(slotNums.begin(), slotNums.end(), s);
        if(it == slotNums.end()){
            slotNums.push_back(s);
            slots.push_back(data);
            numSlots++;
        }
    };
};

#endif
