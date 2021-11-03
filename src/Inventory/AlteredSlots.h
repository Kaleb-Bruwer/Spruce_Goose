#pragma once

#include <map>
#include <algorithm>

#include "Slot.h"

using namespace std;

//Used to simplify the tracking of altered slots
//Instead of checking return values everywhere, simply pass this class by reference
class AlteredSlots{
public:
    map<int, Slot> altered;
    int offset = 0;
    bool hotbarOnly = false;

    void add(int index, Slot s){
        if(hotbarOnly && (index < 36 || index > 44))
            return;

        // cout << "Altered " << index << endl;
        index += offset;
        altered[index] = s;
    }

    void setOffset(int o){
        offset = o;
    };

    void clear(){
        altered.clear();
    }
};
