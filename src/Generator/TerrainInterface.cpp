#include "TerrainInterface.h"

#include "WorldGenerator.h"
#include "../TerrainPort/TerrainPort.h"

TerrainInterface* TerrainInterface::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new WorldGenerator();
            // instance = new TerrainPort();
        }
        constructMutex.unlock();
    }
    return instance;
}
