#include "InventoryControl.h"

#include "../Protocol/PlayerConnection.h"

#include "../JobTickets/WorldToProtocol/SendWindowItem.h"
#include "../JobTickets/WorldToProtocol/ConfirmTransaction.h"

#include <algorithm>

using namespace std;

void InventoryControl::clickWindow(ClickWindowJob* job, bool creative){
    if(activeBlock){
        activeBlock->clickWindow(job, &inventory, alteredSlots, creative);
    }
    else{
        inventory.clickWindow(job, &inventory, alteredSlots, creative);
    }

    ConfirmTransaction* job2 = new ConfirmTransaction();
    job2->windowID = job->windowID;
    job2->actionNum = job->actionNum;

    conn->pushJobToPlayer(job2);

}


void InventoryControl::sendWindowUpdate(PlayerConnection* conn){
    if(alteredSlots.altered.empty())
        return;

    SendWindowItem* job = new SendWindowItem();

    for(auto it = alteredSlots.altered.begin(); it != alteredSlots.altered.end(); it++){
        job->addSlot(it->first, it->second);
        cout << "Added " << it->second << " to job " << job << endl;
    }

    conn->pushJobToPlayer(job);

    alteredSlots.clear();
}

void InventoryControl::openBlock(BlockData* b){
    if(activeBlock)
        closeBlock();

    activeBlock = b;
    b->open(this);
    windowID = b->getWindowID();

    // This is where I sent a OpenCloseWindow job in the old Inventory
}



void InventoryControl::closeBlock(bool byPlayer){
    if(!activeBlock)
        return;

    // In old Inventory, I sent an OpenCloseWindow job if !byPlayer

    if(byPlayer){
        //In the other case, this gets called from BlockData's destructor
    //So it isn't explicitly closed since it's already getting deleted
    //and currently using the vector it keeps Inventories in
        activeBlock->close(this);
    }
    activeBlock = 0;
    windowID = -1;

}
