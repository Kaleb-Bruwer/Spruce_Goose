#include "BlockDataS.h"

#include <cmath>

#include "../../Entities/Item.h"

using namespace std;

template class BlockDataS<3>;  //Furnace
template class BlockDataS<10>; //Crafting table
template class BlockDataS<27>; //Single chest
template class BlockDataS<45>; //Inventory
template class BlockDataS<54>; //Double chest

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

template <int nSlots>
int BlockDataS<nSlots>::availableSpace(Slot in, int start, int end){
    int total = 0;

    int m = in.maxStackSize();
    if(end < start){
        int temp = start;
        start = end;
        end = temp;
    }

    for(int i= start; i<=end; i++){
        if(slots[i].isEmpty()){
            total += m;
        }
        else if(slots[i].typeMatch(in)){
            total += m - slots[i].itemCount;
        }
    }
    return total;
}

template <int nSlots>
vector<Item*> BlockDataS<nSlots>::getDrops(Coordinate<int> itemPos){
    vector<Item*> result;

    for(int i=0; i<nSlots; i++){
        if(!slots[i].isEmpty()){
            Item* item = new Item(slots[i]);
            item->setPos(itemPos);
            result.push_back(item);
        }
    }

    return result;
}
