#include "ToolSpeedTable.h"

ToolSpeedTable* ToolSpeedTable::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new ToolSpeedTable();
        }
        constructMutex.unlock();
    }
    return instance;
}
