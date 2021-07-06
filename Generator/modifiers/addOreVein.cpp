#include "addOreVein.h"

#include <vector>
#include <algorithm>

#include "../../World/Chunk.h"

using namespace std;

vector<Coordinate<int>> getAdjacent(Coordinate<int> pos){
    vector<Coordinate<int>> result;

    pos.x++;
    result.push_back(pos);
    pos.x-=2;
    result.push_back(pos);
    pos.x++;

    pos.y++;
    result.push_back(pos);
    pos.y-=2;
    result.push_back(pos);
    pos.y++;

    pos.z++;
    result.push_back(pos);
    pos.z-=2;
    result.push_back(pos);
    pos.z++;

    return result;
}

int countAdjacentOres(Chunk* c, Coordinate<int> pos, Block b){
    //Different definition of adjacent from getAdjacent, includes corners

}

void addOreVein(Chunk* c, Coordinate<int> pos, int size, Block b){
    vector<Coordinate<int>> vein;
    if(c->getBlock(pos).id == 1){//if stone
        c->setBlock(pos, b);
        size--;
    }
    vein.push_back(b);//If source isn't valid, more immediately-surrounding
    //blocks will still be tried

    //Gets all stone blocks that touch the ore vein
    vector<Coordinate<int>> adjacent;
    vector<int> weight;
    for(Coordinate<int> p : vein){
        vector<Coordinate<int>> pAdj = getAdjacent(p);

        for(Coordinate<int> a : pAdj){
            if(c->getBlock(a).id == 1){
                if(find(adjacent.begin(), adjacent.end(), a) == adjacent.end())
                    adjacent.push_back(a);
            }
        }
    }
    //if 0, you have nothing to work with
    if(adjacent.size() == 0)
        return;

    //Add weights to blocks;

}
