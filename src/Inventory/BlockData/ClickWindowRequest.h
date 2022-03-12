#pragma once

#include "../AlteredSlots.h"

class ClickWindowJob;
class Inventory2;
class ThreadArea;

struct ClickWindowRequest{
    // WARNING: This struct doesn't handle job pickup/drop, do it outside
    ClickWindowJob* job;
    Inventory2* inv;
    AlteredSlots *altered;
    bool creative;

    //As of writing, only needed by Furnace,
    // which is also the reason for this struct existing
    ThreadArea* tArea = 0;
};
