#include "BlockStrengths.h"

BlockStrengths* BlockStrengths::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new BlockStrengths();
        }
        constructMutex.unlock();
    }
    return instance;
}
