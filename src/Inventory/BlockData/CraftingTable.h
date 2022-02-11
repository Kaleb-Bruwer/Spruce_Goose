#ifndef CRAFTINGTABLE_H
#define CRAFTINGTABLE_H

#include "BlockDataS.h"

using namespace std;

class Inventory2;

//Type specifies how many slots class has
class CraftingTable : public BlockDataS<10>{
protected:
    //uses inv for addAlteredSlot
    void craft(bool max, AlteredSlots &altered, int m = 0);
    void checkCrafting(AlteredSlots &altered);

    void mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots & altered);
public:
    CraftingTable() : BlockDataS<10>(false){};


    int getWindowID(){return 1;};
    BlockData* clone();

    vector<Slot> close(InventoryControl*, AlteredSlots&, Inventory2*);

    vector<Slot> clickWindow(ClickWindowRequest request);

    BlockDataType getType(){return CRAFTINGTABLE;};
};

#endif
