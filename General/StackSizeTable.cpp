#include"StackSizeTable.h"

StackSizeTable* StackSizeTable::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new StackSizeTable();
        }
        constructMutex.unlock();
    }
    return instance;
}

//64 is the default value
int StackSizeTable::getStackSize(int id){
    if(id < 256)
        return 64;

    if((id >= 267 && id <= 279) ||
            (id >= 298 && id <= 317) ||
            (id >= 2256 && id <=2267))
        return 1;

    if(size1.find(id) != size1.end())
        return 1;
    if(size16.find(id) != size16.end())
        return 16;

    return 64;
}
