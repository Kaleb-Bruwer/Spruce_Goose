#ifndef CHEST_H
#define CHEST_H

#include "BlockDataS.h"

using namespace std;

class Inventory2;

//This class represents the contents of a single chest
class Chest : public BlockDataS<27>{
public:
    int getWindowID(){return 0;};
    BlockData* clone();

    vector<Slot> clickWindow(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered, bool creative);

};

#endif
