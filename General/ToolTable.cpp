#include "ToolTable.h"

ToolTable* ToolTable::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new ToolTable();
        }
        constructMutex.unlock();
    }
    return instance;
}
