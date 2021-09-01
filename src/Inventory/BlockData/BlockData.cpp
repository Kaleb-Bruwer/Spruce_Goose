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
