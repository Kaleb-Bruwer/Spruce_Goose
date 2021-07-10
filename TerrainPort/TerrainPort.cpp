#include "TerrainPort.h"

using namespace std;

TerrainPort::TerrainPort(){
    myThread = new thread(&TerrainPort::run, this);
}

TerrainPort::~TerrainPort(){}

TerrainPort* TerrainPort::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new TerrainPort();
        }
        constructMutex.unlock();
    }
    return instance;
}

// Should run for entirety of program lifetime, so memory cleanup isn't really important
void TerrainPort::run(){
    while(!quit){
        // Do stuff here
    }
}
