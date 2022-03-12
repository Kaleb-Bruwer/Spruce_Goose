#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <vector>

#include "../Slot.h"
#include "../AlteredSlots.h"
#include "BlockDataType.h"
#include "../../JobTickets/ProtocolToWorld/ClickWindowJob.h"

#include "ClickWindowRequest.h"

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

    // Specifies if two players will see the same items in a block
    // for example a chest is true while a crafting table is false
    bool sharable;
protected:
    BlockData(bool s){
        sharable = s;
    };
public:

    //Destructor ensures that BlockData is safely disconnected from inventories
    virtual ~BlockData();

    bool getSharable(){
        return sharable;
    }

    //Called from Inventory
    void open(InventoryControl*);
    virtual vector<Slot> close(InventoryControl*, AlteredSlots&, Inventory2*);

    virtual int getWindowID() = 0;
    virtual BlockDataType getType() = 0;
    virtual BlockData* clone() = 0;

    //Returns vector of altered slots, which is tracked by InventoryControl
    virtual vector<Slot> clickWindow(ClickWindowRequest request) = 0;

    int getNumPlayers(){
        return inventories.size();
    };

    // Returns false if there were zero (JobTicket wasn't picked up)
    // Deleting JobTicket will be your responsibility if it's never picked up
    bool sendJobToAllOpenPlayers(JobTicket* job);
};

int tryInsertHalfSlot(Slot& dest, Slot& origin, int stackSize);
int tryInsertEmptySlot(Slot& dest, Slot& origin, int stackSize);

#endif
