#include "BlockDataS.h"

#include <cmath>

using namespace std;

template class BlockDataS<10>; //Crafting table
template class BlockDataS<27>; //Single chest
template class BlockDataS<45>; //Inventory

template <int nSlots>
int BlockDataS<nSlots>::tryInsertHalfSlot(Slot& dest, Slot& origin, int stackSize){
    if(origin.isEmpty() || dest.isEmpty())
        return 0;

    if(!origin.typeMatch(&dest))
        return 0;

    int canTake = stackSize = dest.itemCount;
    if(canTake <= 0)
        return 0;

    int willTake = min(canTake, (int) origin.itemCount);
    dest.itemCount += willTake;
    origin.itemCount -= willTake;

    if(origin.itemCount == 0)
        origin.makeEmpty();

    return willTake;
}

template <int nSlots>
int BlockDataS<nSlots>::tryInsertEmptySlot(Slot& dest, Slot& origin, int stackSize){
    if(!dest.isEmpty() || origin.isEmpty())
        return 0;

    int willTake = min(stackSize, (int)origin.itemCount);
    dest = origin;
    dest.itemCount = willTake;
    origin.itemCount -= willTake;

    return willTake;
}

// template <int nSlots>
// bool BlockDataS<nSlots>::mov(Slot& origin, Inventory2* inv, int slotOffset){
//     int stackSize = origin.maxStackSize();
//
//     for(int i=0; i<nSlots; i++){
//         int change = tryInsertHalfSlot(slots[i], origin, stackSize);
//         if(change > 0){
//             addAlteredSlot(i + slotOffset, inv);
//             if(origin.isEmpty())
//                 return false;
//         }
//     }
//
//     for(int i=0; i<nSlots; i++){
//         int change = tryInsertEmptySlot(slots[i], origin, stackSize);
//         if(change > 0){
//             addAlteredSlot(i + slotOffset, inv);
//             if(origin.isEmpty())
//                 return false;
//         }
//     }
// }


template <int nSlots>
int BlockDataS<nSlots>::mov(Slot& origin, int start, int end, AlteredSlots &altered){
    int startCount = origin.itemCount;

    movHalf(origin, start, end, altered);

    if(origin.isEmpty())
        origin.makeEmpty();
    else{
        movEmpty(origin, start, end, altered);
        if(origin.isEmpty())
            origin.makeEmpty();
    }


    return startCount - origin.itemCount;
}


template <int nSlots>
int BlockDataS<nSlots>::movHalf(Slot& origin, int start, int end, AlteredSlots &altered){
    int stackSize = origin.maxStackSize();
    int startCount = origin.itemCount;

    if(end < start){
        for(int s = start; s >= end; s--){
            if(tryInsertHalfSlot(slots[s], origin, stackSize) > 0)
                altered.add(s, slots[s]);
            if(origin.isEmpty())
                return startCount;
        }
    }
    else{
        for(int s = start; s <= end; s++){
            if(tryInsertHalfSlot(slots[s], origin, stackSize) > 0)
                altered.add(s, slots[s]);
            if(origin.isEmpty())
                return startCount;
        }
    }

    return startCount - origin.itemCount;
}

template <int nSlots>
int BlockDataS<nSlots>::movEmpty(Slot& origin, int start, int end, AlteredSlots &altered){
    int stackSize = origin.maxStackSize();
    int startCount = origin.itemCount;

    if(end < start){
        for(int s = start; s >= end; s--){
            if(tryInsertEmptySlot(slots[s], origin, stackSize) > 0)
                altered.add(s, slots[s]);
            if(origin.isEmpty())
                return startCount;
        }
    }
    else{
        for(int s = start; s <= end; s++){
            if(tryInsertEmptySlot(slots[s], origin, stackSize) > 0)
                altered.add(s, slots[s]);
            if(origin.isEmpty())
                return startCount;
        }
    }

    return startCount - origin.itemCount;
}
