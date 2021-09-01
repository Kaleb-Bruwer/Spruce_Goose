#include "InventoryControl.h"

#include "../Protocol/PlayerConnection.h"

#include "../JobTickets/WorldToProtocol/SendWindowItem.h"
#include "../JobTickets/WorldToProtocol/ConfirmTransaction.h"
#include "../JobTickets/WorldToProtocol/OpenCloseWindow.h"

#include <algorithm>

using namespace std;

vector<Slot> InventoryControl::clickWindow(ClickWindowJob* job, bool creative){
    vector<Slot> dropped;
    if(activeBlock){
        dropped = activeBlock->clickWindow(job, &inventory, alteredSlots, creative);
    }
    else{
        dropped = inventory.clickWindow(job, &inventory, alteredSlots, creative);
    }

    ConfirmTransaction* job2 = new ConfirmTransaction();
    job2->windowID = job->windowID;
    job2->actionNum = job->actionNum;

    conn->pushJobToPlayer(job2);

    return dropped;
}


void InventoryControl::sendWindowUpdate(){
    if(alteredSlots.altered.empty())
        return;

    SendWindowItem* job = new SendWindowItem();

    for(auto it = alteredSlots.altered.begin(); it != alteredSlots.altered.end(); it++){
        job->addSlot(it->first, it->second);
    }

    conn->pushJobToPlayer(job);

    alteredSlots.clear();
}

void InventoryControl::openBlock(BlockData* b){
    if(activeBlock)
        closeBlock();

    // Non-sharable blocks like crafting tables get their own copy for each player
    if(b->getSharable())
        activeBlock = b;
    else
        activeBlock = b->clone();

    activeBlock->open(this);
    windowID = activeBlock->getWindowID();

    OpenCloseWindow* job = new OpenCloseWindow();
    job->open = true;
    job->windowID = windowID;

    switch(job->windowID){
    case 0:
        job->name = "Chest";
        job->numSlots = 27;
        break;

    case 1:
        job->name = "Crafting Table";
        job->numSlots = 10;
        break;

    }

    conn->pushJobToPlayer(job);
}



vector<Slot> InventoryControl::closeBlock(bool byPlayer){
    vector<Slot> dropped;
    if(!activeBlock){
        //inventory
        dropped = inventory.close(this, alteredSlots, &inventory);
        sendWindowUpdate();
        return dropped;
    } // In old Inventory, I sent an OpenCloseWindow job if !byPlayer
    else if(byPlayer){
        //In the other case, this gets called from BlockData's destructor
        //So it isn't explicitly closed since it's already getting deleted
        //and currently using the vector it keeps Inventories in
        dropped = activeBlock->close(this, alteredSlots, &inventory);
        sendWindowUpdate();
    }
    if(!activeBlock->getSharable())
        delete activeBlock;

    activeBlock = 0;
    windowID = -1;

    return dropped;
}
