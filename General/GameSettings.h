#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <mutex>
#include <map>
#include <vector>
#include <string>

#include "../Datastructures/NBT/Tag_Compound.h"

using namespace std;

struct WorldBorder{
    double diameter;
    double speed; //m per tick
};

class GameSettings{
private:

    GameSettings(){};

    inline static GameSettings* instance = 0;
    inline static mutex constructMutex;

    bool isHardcore = false;

    //Currently assumed to only have one dimension
    //Multiworld not figured out at time of writing
    int worldCount = 1;
    vector<string> worldNames = {"minecraft:overworld"};

    long long hashedSeed = 0;
    int maxPlayers = 10000;

    int renderDistance = 12;
    bool enableRespawnScreen = true;
    bool isDebug = false;
    bool isFlat = false;

    WorldBorder worldBorder;
    Tag_Compound* dimensionCodec = 0;
    Tag_Compound* dimension = 0;


public:
    static GameSettings* getInstance();

    bool getIsHardcore(){return isHardcore;};
    int getWorldCount(){return worldCount;};
    vector<string> getWorldNames(){return worldNames;};
    long long getHashedSeed(){return hashedSeed;};
    int getMaxPlayers(){return maxPlayers;};
    bool getEnableRespawnScreen(){return enableRespawnScreen;};
    bool getIsDebug(){return isDebug;};
    bool getIsFlat(){return isFlat;};

    Tag_Compound* getDimensionCodec();
    Tag_Compound* getDimension();

};

#endif
