#pragma once

#include "BlockDataS.h"

class Inventory2;
class ThreadArea;

class Furnace : public BlockDataS<3>{
// slots:
//     0: input
//     1: fuel
//     2: output

private:
    unsigned long long burnFinish = 0; //0 means no burn
    unsigned long long fuelFinish = 0;

    vector<Slot> clickMode2(int clicked, int btn, Inventory2* inv,
            AlteredSlots &altered, bool creative);
    void clickMode6(int clicked, int btn, Inventory2* inv, AlteredSlots& altered);
    void mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered);
public:
    Furnace() : BlockDataS<3>(true){};

    int getWindowID(){return 2;};

    vector<Slot> clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative);

    BlockData* clone();
    BlockDataType getType(){return FURNACE;};

    // These can be called in any order if occuring on same tick
    void burnCallback(ThreadArea* tArea);
    void fuelCallback(ThreadArea* tArea);

    // Takes fuel source and sets fuelFinish appropriately
    // sets fuelFinish to 0 if none available
    void startNextFuel(ThreadArea* tArea);

};

void burnCallbackWrap(void* obj, ThreadArea* tArea);

void fuelCallbackWrap(void* obj, ThreadArea* tArea);
