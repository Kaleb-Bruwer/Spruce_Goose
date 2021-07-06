#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <vector>

#include "../Slot.h"
#include "../../JobTickets/ProtocolToWorld/ClickWindowJob.h"
#include "../AlteredSlots.h"

using namespace std;

class Inventory2;
class InventoryControl;

class BlockData{
private:
    //List of all inventories that have this BlockData open
    //This is only used to notify them if this BlockData gets deleted
    //(if a chest is broken while you have it open, for example)

    //This vector doesn't get, and shouldn't be, cloned
    vector<InventoryControl*> inventories;
protected:
public:

    //Destructor ensures that BlockData is safely disconnected from inventories
    virtual ~BlockData();

    //Called from Inventory
    void open(InventoryControl*);
    void close(InventoryControl*);

    virtual int getWindowID() = 0;
    virtual BlockData* clone() = 0;

    //Returns vector of altered slots, which is tracked by InventoryControl
    virtual void clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative) = 0;
};

#endif
