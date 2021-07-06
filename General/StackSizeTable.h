#ifndef STACKSIZETABLE_H
#define STACKSIZETABLE_H

#include <mutex>
#include <set>

using namespace std;

class StackSizeTable{
private:
    //Only contains items not already handled by if ranges in getStackSize()
    set<int> size16 = {323, 325, 332, 335, 344,368};
    set<int> size1 = {256, 257, 258, 259, 261, 282, 263, 284, 285, 286,
        290, 291, 292, 293, 294, 324, 326, 327, 328, 329, 330, 333, 342,
        343, 346, 354, 355, 359, 373, 386, 387, 398, 403, 416, 417, 418,
        419, 422, 436, 437, 438, 441, 442, 443, 444, 445, 446, 447, 447,
        449};

    StackSizeTable(){};
    inline static StackSizeTable* instance = 0;
    inline static mutex constructMutex;
public:
    static StackSizeTable* getInstance();

    int getStackSize(int id);
};

#endif
