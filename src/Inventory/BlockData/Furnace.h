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

    bool hasProgressCallback = false;

    vector<Slot> clickMode2(int clicked, int btn, Inventory2* inv,
            AlteredSlots &altered, bool creative);
    void clickMode6(int clicked, int btn, Inventory2* inv, AlteredSlots& altered);
    void mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered);

    // These can be called in any order if occuring on same tick
    void burnCallback(ThreadArea* tArea);
    void fuelCallback(ThreadArea* tArea);
    void progressCallback(ThreadArea* tArea);

    // Takes fuel source and sets fuelFinish appropriately
    // sets fuelFinish to 0 if none available
    void startNextFuel(ThreadArea* tArea);
    void startBurn(ThreadArea* tArea);
    void cancelBurn(unsigned long long);
    void sendWindowProperty(unsigned long long tick);
    void sendSlots();

    void setProgressCallback(ThreadArea* tArea);

    friend void burnCallbackWrap(void* obj, ThreadArea* tArea);
    friend void fuelCallbackWrap(void* obj, ThreadArea* tArea);
    friend void progressCallbackWrap(void* obj, ThreadArea* tArea);

public:
    Furnace() : BlockDataS<3>(true){};

    // Used for EntityStore to know when to send window updates
    // This is only for callbacks, clickWindow handles its own updates
    bool altered = false;

    int getWindowID(){return 2;};

    vector<Slot> clickWindow(ClickWindowRequest request);

    BlockData* clone();
    BlockDataType getType(){return FURNACE;};

    unsigned long long getBurnFinish(){return burnFinish;};
    unsigned long long getFuelFinish(){return fuelFinish;};


};

void burnCallbackWrap(void* obj, ThreadArea* tArea);
void fuelCallbackWrap(void* obj, ThreadArea* tArea);
void progressCallbackWrap(void* obj, ThreadArea* tArea);
