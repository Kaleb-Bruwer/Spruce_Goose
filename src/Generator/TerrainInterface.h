#pragma once

#include <mutex>

#include "../JobTickets/JobTicket.h"

// Child classes must friend TerrainInterface to allow access to (private) constructor

class TerrainInterface{
private:
    inline static TerrainInterface* instance = 0;
    inline static std::mutex constructMutex;

public:
    // change this function in TerrainInterface.cpp to switch between implementations
    static TerrainInterface* getInstance();

    virtual void pushJob(JobTicket* job) = 0;

};
