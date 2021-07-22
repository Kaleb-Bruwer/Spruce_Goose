#include "BlockData.h"

#include "../Inventory2.h"
#include "../InventoryControl.h"

using namespace std;

BlockData::~BlockData(){
    for(auto it = inventories.begin(); it != inventories.end(); it++){
        //false means not closed by player
        (*it)->closeBlock(false);
    }
}

void BlockData::open(InventoryControl* inv){
    inventories.push_back(inv);
}

void BlockData::close(InventoryControl* inv){
    for(auto it = inventories.begin(); it != inventories.end(); it++){
        if(*it == inv){
            inventories.erase(it);
            return;
        }
    }
}
