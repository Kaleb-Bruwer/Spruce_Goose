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

    // void add(AlteredSlot a){
    //     a.index += offset;
    //
    //     altered[a.index] = a.slot;
    //
    //     // if(find(altered.begin(), altered.end(), a) == altered.end())
    //     //     altered.push_back(a);
    // };

    void add(int index, Slot s){
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
