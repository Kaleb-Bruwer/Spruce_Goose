#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <mutex>

using namespace std;

class LevelData{
private:
    inline static LevelData* instance = 0;
    inline static mutex constructMutex;
    LevelData(){};

    int gamemode = 0; //0=survival, 1=creative

public:
    static LevelData* getInstance();

    int getGamemode(){return gamemode;};
};

#endif
