#ifndef CHEST_H
#define CHEST_H

#include "BlockDataS.h"

class Inventory2;

//This class represents the contents of a single chest
template <int nSlots>
class BaseChest : public BlockDataS<nSlots>{
private:
    void mouseDrag(ClickWindowJob* job, Inventory2* inv, AlteredSlots &altered);

public:
    BaseChest() : BlockDataS<nSlots>(true){};

    int getWindowID(){return 0;};

    vector<Slot> clickWindow(ClickWindowRequest request);

};

class ChestSingle : public BaseChest<27>{
public:
    BlockData* clone();

    BlockDataType getType(){return CHESTSINGLE;};

};

class ChestDouble : public BaseChest<54>{
public:
    ChestDouble(){};
    ChestDouble(ChestSingle& c1, ChestSingle& c2);

    BlockData* clone();

    void splitChest(ChestSingle& c1, ChestSingle& c2);

    BlockDataType getType(){return CHESTDOUBLE;};
};

#endif
