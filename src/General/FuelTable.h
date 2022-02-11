#pragma once

#include <mutex>
#include <unordered_map>

class FuelTable{
private:
    std::unordered_map<short, short> itemIDToDuration;

    inline static FuelTable* instance = 0;
    inline static std::mutex constructMutex;

    FuelTable();
    void readFromFile();

public:
    static FuelTable* getInstance();

    int getDuration(int itemID);

};
