#pragma once

#include "BlockDataS.h"

class Inventory2;

class Furnace : public BlockDataS<3>{
private:
    vector<Slot> clickMode2(int clicked, int btn, Inventory2* inv,
            AlteredSlots &altered, bool creative);
public:

    int getWindowID(){return 2;};

    vector<Slot> clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative);

    BlockData* clone();
    BlockDataType getType(){return FURNACE;};

};
