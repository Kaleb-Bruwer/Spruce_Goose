#include "EIDAllocator.h"

#include <iostream>

using namespace std;

EIDAllocator::EIDAllocator(){

}

EIDAllocator* EIDAllocator::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new EIDAllocator();
        }
        constructMutex.unlock();
    }
    return instance;
}

int EIDAllocator::getID(){
    lock.lock();
    int id;

    if(releasedIDs.size() > 0){
        id = releasedIDs[releasedIDs.size()-1];
        releasedIDs.pop_back();
    }
    else{
        id = nextID;
        nextID++;
    }
    lock.unlock();
    return id;
}

void EIDAllocator::releaseID(int id){
    lock.lock();
    releasedIDs.push_back(id);
    lock.unlock();
}
