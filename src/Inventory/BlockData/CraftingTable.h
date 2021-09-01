#ifndef CRAFTINGTABLE_H
#define CRAFTINGTABLE_H

#include "BlockDataS.h"

using namespace std;

class Inventory2;

//Type specifies how many slots class has
class CraftingTable : public BlockDataS<10>{
protected:
    //uses inv for addAlteredSlot
    void craft(bool max, AlteredSlots &altered);
    void checkCrafting(AlteredSlots &altered);

    void mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots & altered);
public:
    int getWindowID(){return 1;};
    BlockData* clone();

    vector<Slot> close(InventoryControl*, AlteredSlots&, Inventory2*);

    // void clickWindow(ClickWindowJob* job, Inventory* inv, bool creative);
    vector<Slot> clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative);
};

#endif
