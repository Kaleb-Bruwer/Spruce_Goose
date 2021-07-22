#ifndef TOOLSPEEDTABLE_H
#define TOOLSPEEDTABLE_H

#include <mutex>
#include <map>

using namespace std;

class ToolSpeedTable{
private:

    map<int, float> table = {
        {269, 2}, //wood tools
        {270, 2},
        {271, 2},
        {290, 2},

        {273, 4}, //stone tools
        {274, 4},
        {275, 4},
        {291, 4},

        {256, 6}, //iron tools
        {257, 6},
        {258, 6},
        {292, 6},

        {284, 12}, //gold tools
        {285, 12},
        {286, 12},
        {294, 12},

        {277, 8}, //diamond tools
        {278, 8},
        {279, 8},
        {293, 8},

        {268, 1.5}, //swords
        {272, 1.5},
        {283, 1.5},
        {267, 1.5},
        {276, 1.5},

        {359, 1.5} //shear
    };

    ToolSpeedTable(){};

    inline static ToolSpeedTable* instance = 0;
    inline static mutex constructMutex;
public:
    static ToolSpeedTable* getInstance();

    float getVal(int t){
        auto it = table.find(t);
        if(it != table.end()){
            return it->second;
        }
        //Random items that aren't tools
        return 1;
    };
};

#endif
