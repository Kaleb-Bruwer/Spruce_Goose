#include "Slot.h"

#include "../General/StackSizeTable.h"

using namespace std;

int Slot::maxStackSize(){
    //Maybe optimize this by remembering size if this gets used too often
    StackSizeTable* table = StackSizeTable::getInstance();
    return table->getStackSize(itemID);
}

bool Slot::typeMatch(Slot* rhs){
    return (itemID == rhs->itemID)
        && (itemDamage == rhs->itemDamage
            || itemDamage == -1 || rhs->itemDamage == -1);
}

bool Slot::fullMatch(Slot* rhs){
    return (itemID == rhs->itemID
        && itemDamage == rhs->itemDamage
        && itemCount == rhs->itemCount);
}

bool Slot::typeMatch(Slot &rhs){
    return (itemID == rhs.itemID)
        && (itemDamage == rhs.itemDamage
            || itemDamage == -1 || rhs.itemDamage == -1);
}

bool Slot::fullMatch(Slot &rhs){
    return (itemID == rhs.itemID
        && itemDamage == rhs.itemDamage
        && itemCount == rhs.itemCount);
}

bool operator< (const Slot &lhs, const Slot &rhs){
    if(lhs.itemID < rhs.itemID)
        return true;
    if(lhs.itemID > rhs.itemID)
        return false;

    if(lhs.itemDamage >=0 && rhs.itemDamage >= 0){
        if(lhs.itemDamage < rhs.itemDamage)
            return true;
        if(lhs.itemDamage > rhs.itemDamage)
            return false;
    }

    if(lhs.itemCount < rhs.itemCount)
        return true;

    return false;
}

bool operator== (const Slot &lhs, const Slot &rhs){
    if(lhs.itemID != rhs.itemID)
        return false;

    if(lhs.itemDamage >=0 && rhs.itemDamage >= 0){
        if(lhs.itemDamage != rhs.itemDamage)
            return false;
    }

    return (lhs.itemCount == rhs.itemCount);
}

ostream& operator <<(ostream &out, const Slot &rhs){
    out << "Slot:(" << rhs.itemID << ", " << rhs.itemDamage << ", ";
    out << rhs.itemCount << ")";
    return out;
}
