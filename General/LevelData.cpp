#include "LevelData.h"

LevelData* LevelData::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new LevelData();
        }
        constructMutex.unlock();
    }
    return instance;
}
