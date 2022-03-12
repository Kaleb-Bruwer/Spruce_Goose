#include "BlockData.h"

#include "../Inventory2.h"
#include "../InventoryControl.h"

using namespace std;

BlockData::~BlockData(){
    // Notifies all players that have block open that it has been closed
    for(auto it = inventories.begin(); it != inventories.end(); it++){
        //false means not closed by player
        (*it)->closeBlock(false);
    }
}

void BlockData::open(InventoryControl* inv){
    inventories.push_back(inv);
}

vector<Slot> BlockData::close(InventoryControl* invCont, AlteredSlots& altered, Inventory2* inv){
    for(auto it = inventories.begin(); it != inventories.end(); it++){
        if(*it == invCont){
            inventories.erase(it);
            return vector<Slot>();
        }
    }
}


bool BlockData::sendJobToAllOpenPlayers(JobTicket* job){
    if(inventories.empty())
        return false;

    for(int i=0; i<inventories.size();i++){
        inventories[i]->sendJobToPlayer(job);
    }

    return true;
}

int tryInsertHalfSlot(Slot& dest, Slot& origin, int stackSize){
    if(origin.isEmpty() || dest.isEmpty())
        return 0;

    if(!origin.typeMatch(&dest))
        return 0;

    int canTake = stackSize - dest.itemCount;
    if(canTake <= 0)
        return 0;

    int willTake = min(canTake, (int) origin.itemCount);
    dest.itemCount += willTake;
    origin.itemCount -= willTake;

    if(origin.itemCount == 0)
        origin.makeEmpty();

    return willTake;
}

int tryInsertEmptySlot(Slot& dest, Slot& origin, int stackSize){
    if(!dest.isEmpty() || origin.isEmpty())
        return 0;

    int willTake = min(stackSize, (int)origin.itemCount);
    dest = origin;
    dest.itemCount = willTake;
    origin.itemCount -= willTake;

    return willTake;
}
