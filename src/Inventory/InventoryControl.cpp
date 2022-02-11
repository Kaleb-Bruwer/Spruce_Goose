#include "InventoryControl.h"

#include "../Protocol/PlayerConnection.h"

#include "../JobTickets/WorldToProtocol/SendWindowItem.h"
#include "../JobTickets/WorldToProtocol/ConfirmTransaction.h"
#include "../JobTickets/WorldToProtocol/OpenCloseWindow.h"
#include "../JobTickets/SendPacket.h"

#include "../Protocol/AdvancedWriter.h"

#include <algorithm>

using namespace std;

vector<Slot> InventoryControl::clickWindow(ClickWindowRequest request){
    request.inv = &inventory;
    request.altered = &alteredSlots;

    vector<Slot> dropped;
    if(activeBlock){
        dropped = activeBlock->clickWindow(request);
    }
    else{
        dropped = inventory.clickWindow(request);
    }

    ConfirmTransaction* job2 = new ConfirmTransaction();
    job2->windowID = request.job->windowID;
    job2->actionNum = request.job->actionNum;

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

    string title;
    short int numSlots = 0;

    switch(activeBlock->getType()){
    case CHESTSINGLE:
        title = "Chest";
        numSlots = 27;
        break;
    case CHESTDOUBLE:
        title = "Chest";
        numSlots = 54;
        break;
    case CRAFTINGTABLE:
        title = "Crafting table";
        numSlots = 10;
    }

    // If applicable, send contents of block
    AdvancedWriter writer;


    // OpenWindow and Window Items must have the same window ID, not to be
    // confused with the window type
    short winID = 1;
    writer.writeOpenWindow(winID, windowID, title, numSlots, false);
    bool didWrite = writer.writeWindowItems(&inventory, b, winID);

    conn->pushJobToPlayer(new SendPacket(&writer));
    // // conn->pushJobToPlayer(job);
    // if(didWrite){
    //     cout << "Sending window items\n";
    // }

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
