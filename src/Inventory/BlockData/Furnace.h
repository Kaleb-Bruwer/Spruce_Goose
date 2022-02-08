#pragma once

#include "BlockDataS.h"

class Inventory2;

class Furnace : public BlockDataS<3>{
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
    void burnCallback();
    void fuelCallback();

    // Takes fuel source and sets fuelFinish appropriately
    // sets fuelFinish to 0 if none available
    void startNextFuel();

};

void burnCallbackWrap(void* obj);

void fuelCallbackWrap(void* obj);
