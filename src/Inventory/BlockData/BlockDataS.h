#ifndef BLOCKDATAS_H
#define BLOCKDATAS_H

#include "BlockData.h"
#include "../AlteredSlots.h"
#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class Inventory;
class Item;

template <int nSlots>
class BlockDataS : public BlockData{
protected:
    BlockDataS(bool s) : BlockData(s){};
public:
// Protected wouldn't be accessible over different sized BlockDataS,
// for example chest would not be able to use Inventory2
    Slot slots[nSlots];

    //Inserts in slots, iterating from start to end
    //Overload for classes where this isn't appropriate
    //Inventory used to call addAlteredSlot
    //slotOffset is normally 0 since this isn't the inventory side
    //virtual bool mov(Slot& origin, Inventory* inv, int slotOffset = 0);
    int mov(Slot& origin, int start, int end, AlteredSlots &altered);
    virtual int movHalf(Slot& origin, int start, int end, AlteredSlots &altered);
    virtual int movEmpty(Slot& origin, int start, int end, AlteredSlots &altered);

    int availableSpace(Slot in, int start, int end);

    // If this item is broken, this is what will be dropped
    // Don't call this directly on a double chest, unless you broke both halves
    // param itemPos: sets the position for each item

    vector<Item*> getDrops(Coordinate<int> itemPos);

};

#endif
