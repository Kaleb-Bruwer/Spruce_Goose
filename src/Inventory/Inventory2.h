#pragma once

#include "BlockData/BlockDataS.h"

using namespace std;

class Item;

enum DragMode{
    LEFT,
    RIGHT,
    NONE
};

struct DragData{
    DragMode dragMode;

    vector<int> dragSlots;
    vector<int> baseCount;

    int dragTotal = 0;
};

// Was named Inventory2 to distinguish it from Inventory while I was refactoring
// the inventory system
class Inventory2 : public BlockDataS<45>{
protected:

    //0: only craft 1 unit. 1:craft maximum possible
    void craft(bool max, AlteredSlots &altered);

    //Calls Crafting class and sets slots[0] to the appropriate product
    //Returns true if crafting product is altered
    void checkCrafting(AlteredSlots &altered);

public:
    static const int numSlots = 45;
    Slot hover;
    short cursor = 36; //range: [36,44] held item

    DragData dragData;

    int getWindowID(){return 0;};
    BlockData* clone();

    //Inventory parameter not used, only there for polymorphism
    vector<Slot> clickWindow(ClickWindowJob*, Inventory2* inv, AlteredSlots &altered, bool creative);

    int tryPickup(Item* item, AlteredSlots& altered);

    //most parameters not used, only there for polymorphism
    vector<Slot> close(InventoryControl*, AlteredSlots&, Inventory2*);


};
